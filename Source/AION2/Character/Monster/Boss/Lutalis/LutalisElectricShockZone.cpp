// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Monster/Boss/Lutalis/LutalisElectricShockZone.h"

#include "Character/AOCharacter.h"
#include "Character/Daeva/Daeva.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"

ALutalisElectricShockZone::ALutalisElectricShockZone()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(30.f);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	WarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningMesh"));
	WarningMesh->SetupAttachment(SceneRoot);
	WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StrikeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StrikeNiagaraComponent"));
	StrikeNiagaraComponent->SetupAttachment(SceneRoot);
	StrikeNiagaraComponent->SetAutoActivate(false);

	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(SceneRoot);
	DamageSphere->SetGenerateOverlapEvents(true);
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	WarningMesh->SetHiddenInGame(true);
	StrikeNiagaraComponent->SetHiddenInGame(true);
	StrikeNiagaraComponent->SetVisibility(false, true);
}

void ALutalisElectricShockZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALutalisElectricShockZone, Radius);
	DOREPLIFETIME(ALutalisElectricShockZone, VisualState);
	DOREPLIFETIME(ALutalisElectricShockZone, ReplicatedWarningDuration);
}

void ALutalisElectricShockZone::BeginPlay()
{
	Super::BeginPlay();

	ApplyZoneDimensions();
	OnRep_VisualState();
}

void ALutalisElectricShockZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWarning)
	{
		UpdateWarning(DeltaTime);
	}

	DrawDamageDebug();
}

void ALutalisElectricShockZone::InitZone(AAOCharacter* InDamageCauser, const FAttackData& InDamageData, float InRadius)
{
	DamageCauser = InDamageCauser;
	DamageData = InDamageData;
	Radius = FMath::Max(InRadius, 1.f);

	ApplyZoneDimensions();
	ForceNetUpdate();
}

void ALutalisElectricShockZone::StartWarning(float InWarningDuration)
{
	if (!HasAuthority())
	{
		return;
	}

	ReplicatedWarningDuration = FMath::Max(InWarningDuration, 0.1f);
	VisualState = ELutalisElectricShockZoneVisualState::Warning;
	StartWarningInternal(ReplicatedWarningDuration);
	ForceNetUpdate();
}

void ALutalisElectricShockZone::ActivateStrike()
{
	if (!HasAuthority())
	{
		return;
	}

	VisualState = ELutalisElectricShockZoneVisualState::Strike;
	ActivateStrikeInternal();
	ForceNetUpdate();
}

void ALutalisElectricShockZone::OnRep_VisualState()
{
	switch (VisualState)
	{
	case ELutalisElectricShockZoneVisualState::Warning:
		StartWarningInternal(ReplicatedWarningDuration);
		break;
	case ELutalisElectricShockZoneVisualState::Strike:
		ActivateStrikeInternal();
		break;
	default:
		bWarning = false;
		SetWarningVisible(false);
		SetStrikeVisible(false);
		DamageSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RefreshTickEnabled();
		break;
	}
}

void ALutalisElectricShockZone::OnRep_Radius()
{
	ApplyZoneDimensions();
}

void ALutalisElectricShockZone::StartWarningInternal(float InWarningDuration)
{
	WarningDuration = FMath::Max(InWarningDuration, 0.1f);
	WarningElapsed = 0.f;
	bWarning = true;
	DamagedActors.Reset();

	ApplyZoneDimensions();

	if (WarningMaterial)
	{
		WarningMesh->SetMaterial(0, WarningMaterial);
	}

	WarningMID = WarningMesh->CreateDynamicMaterialInstance(0);
	if (WarningMID)
	{
		WarningMID->SetScalarParameterValue(FillParameterName, 0.f);
	}

	SetStrikeVisible(false);
	SetWarningVisible(true);
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RefreshTickEnabled();
}

void ALutalisElectricShockZone::ActivateStrikeInternal()
{
	bWarning = false;

	SetWarningVisible(false);
	SetStrikeVisible(true);

	if (HasAuthority())
	{
		ApplyStrikeDamage();
		SetLifeSpan(FMath::Max(StrikeLifeTime, 0.1f));
	}

	RefreshTickEnabled();
}

void ALutalisElectricShockZone::ApplyZoneDimensions()
{
	const float BaseDiameter = FMath::Max(WarningMeshBaseDiameter, 1.f);
	const float MeshScale = (Radius * 2.f) / BaseDiameter;

	WarningMesh->SetRelativeScale3D(FVector(MeshScale, MeshScale, 1.f));
	StrikeNiagaraComponent->SetRelativeLocation(StrikeEffectRelativeLocation);
	StrikeNiagaraComponent->SetRelativeRotation(StrikeEffectRelativeRotation);
	DamageSphere->SetSphereRadius(Radius, true);
}

void ALutalisElectricShockZone::UpdateWarning(float DeltaTime)
{
	WarningElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(WarningElapsed / WarningDuration, 0.f, 1.f);
	if (WarningMID)
	{
		WarningMID->SetScalarParameterValue(FillParameterName, Alpha);
	}
}

void ALutalisElectricShockZone::ApplyStrikeDamage()
{
	if (!HasAuthority() || !DamageCauser)
	{
		return;
	}

	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageSphere->UpdateOverlaps();

	TArray<AActor*> Overlaps;
	DamageSphere->GetOverlappingActors(Overlaps, ADaeva::StaticClass());

	for (AActor* Actor : Overlaps)
	{
		ADaeva* Target = Cast<ADaeva>(Actor);
		if (!Target || Target->IsDead() || DamagedActors.Contains(Target))
		{
			continue;
		}

		FHitResult HitResult;
		BuildDamageHitResult(Target, HitResult);

		AAOCharacter* TargetCharacter = Target;
		TargetCharacter->TakeDamageAO(DamageData, HitResult, DamageCauser);
		DamagedActors.Add(Target);
	}

	DamageSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALutalisElectricShockZone::BuildDamageHitResult(ADaeva* Target, FHitResult& OutHitResult) const
{
	if (!Target)
	{
		return;
	}

	const FVector TargetLocation = Target->GetActorLocation();

	OutHitResult.HitObjectHandle = FActorInstanceHandle(Target);
	OutHitResult.Location = TargetLocation;
	OutHitResult.ImpactPoint = TargetLocation;
	OutHitResult.Component = Target->GetCapsuleComponent();
	OutHitResult.bBlockingHit = true;

	FVector ImpactNormal = TargetLocation - GetActorLocation();
	ImpactNormal.Z = 0.0f;
	ImpactNormal = ImpactNormal.GetSafeNormal();
	OutHitResult.ImpactNormal = ImpactNormal;
	OutHitResult.Normal = ImpactNormal;
}

void ALutalisElectricShockZone::SetWarningVisible(bool bVisible)
{
	WarningMesh->SetVisibility(bVisible, true);
	WarningMesh->SetHiddenInGame(!bVisible, true);
}

void ALutalisElectricShockZone::SetStrikeVisible(bool bVisible)
{
	if (StrikeNiagaraSystem)
	{
		StrikeNiagaraComponent->SetAsset(StrikeNiagaraSystem);
	}

	StrikeNiagaraComponent->SetVisibility(bVisible, true);
	StrikeNiagaraComponent->SetHiddenInGame(!bVisible, true);

	if (bVisible)
	{
		StrikeNiagaraComponent->Activate(true);
	}
	else
	{
		StrikeNiagaraComponent->DeactivateImmediate();
	}
}

void ALutalisElectricShockZone::RefreshTickEnabled()
{
	SetActorTickEnabled(bWarning || bDrawDamageDebug);
}

void ALutalisElectricShockZone::DrawDamageDebug() const
{
#if ENABLE_DRAW_DEBUG
	if (!bDrawDamageDebug || !GetWorld())
	{
		return;
	}

	const FColor DebugColor =
		VisualState == ELutalisElectricShockZoneVisualState::Strike ? FColor::Red : FColor::Yellow;

	DrawDebugSphere(
		GetWorld(),
		DamageSphere->GetComponentLocation(),
		Radius,
		32,
		DebugColor,
		false,
		DamageDebugDrawDuration,
		0,
		2.0f
	);
#endif
}
