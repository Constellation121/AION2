#include "Character/AOCharacter.h"
#include "Character/AOCharacterMovementComponent.h"
#include "Physics/Collision.h"
#include "GAS/AOGameplayTags.h"
#include "Player/AOPlayerController.h"
#include "GAS/AttributeSet/AOAttributeSet.h"
#include "Actor/AOProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

AAOCharacter::AAOCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAOCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAOCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAOCharacter, bIsDead);
}

void AAOCharacter::Multicast_DrawDebugCapsuleCollider_Implementation(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor)
{
	DrawDebugCapsuleCollider(CapsuleOrigin, CapsuleHalfHeight, AttackRadius, DrawColor);
}

void AAOCharacter::SearchTarget()
{
}

void AAOCharacter::TeleportBackToTarget()
{
}

void AAOCharacter::CheckAttackHit(const FAttackData& AttackData)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][CheckAttackHit][Enter] Actor=%s NetMode=%d HasAuthority=%d Range=%.1f Radius=%.1f StartOffset=%s Direction=%s"),
		*GetNameSafe(this),
		static_cast<int32>(GetNetMode()),
		HasAuthority() ? 1 : 0,
		AttackData.TraceData.Range,
		AttackData.TraceData.Radius,
		*AttackData.TraceData.StartOffset.ToString(),
		*AttackData.TraceData.Direction.ToString()
	);

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][CheckAttackHit][Skip] Actor=%s Reason=NoAuthority"), *GetNameSafe(this));
		return;
	}

	TArray<FHitResult> OutHitResults;

	const float AttackRange = AttackData.TraceData.Range;
	const float AttackRadius = AttackData.TraceData.Radius;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackTrace), false, this);

	FVector SweepStart = GetActorTransform().TransformPosition(AttackData.TraceData.StartOffset);
	FVector SweepEnd = SweepStart + AttackData.TraceData.Direction.GetSafeNormal() * AttackRange;
	FVector CapsuleCenter = SweepStart + (SweepEnd - SweepStart) * 0.5f;

	bool bHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, SweepStart, SweepEnd, FQuat::Identity, CCHANNEL_ATTACK, FCollisionShape::MakeSphere(AttackRadius), Params);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][CheckAttackHit][Sweep] Actor=%s Start=%s End=%s HitDetected=%d HitCount=%d"),
		*GetNameSafe(this),
		*SweepStart.ToString(),
		*SweepEnd.ToString(),
		bHitDetected ? 1 : 0,
		OutHitResults.Num()
	);

	if (CVarDrawAttackTrace.GetValueOnGameThread())
	{
		const float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
		Multicast_DrawDebugCapsuleCollider(CapsuleCenter, CapsuleHalfHeight, AttackRadius, DrawColor);
	}

	if (!bHitDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][CheckAttackHit][NoHit] Actor=%s"), *GetNameSafe(this));
		return;
	}

	bool bDidShakeCamera = false;

	for (const FHitResult& HitResult : OutHitResults)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[DamageTrace][CheckAttackHit][Candidate] Causer=%s HitActor=%s HitComp=%s Impact=%s Distance=%.1f"),
			*GetNameSafe(this),
			*GetNameSafe(HitResult.GetActor()),
			*GetNameSafe(HitResult.GetComponent()),
			*HitResult.ImpactPoint.ToString(),
			HitResult.Distance
		);

		AAOCharacter* HitActor = Cast<AAOCharacter>(HitResult.GetActor());
		if (!IsValid(HitActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][CheckAttackHit][Reject] Causer=%s HitActor=%s Reason=NotAOCharacter"), *GetNameSafe(this), *GetNameSafe(HitResult.GetActor()));
			continue;
		}

		if (HitActor->IsDead())
		{
			UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][CheckAttackHit][Reject] Causer=%s Target=%s Reason=TargetDead"), *GetNameSafe(this), *GetNameSafe(HitActor));
			continue;
		}

		if (!IsEnemy(HitActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][CheckAttackHit][Reject] Causer=%s Target=%s Reason=NotEnemy"), *GetNameSafe(this), *GetNameSafe(HitActor));
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][CheckAttackHit][Accept] Causer=%s Target=%s"), *GetNameSafe(this), *GetNameSafe(HitActor));
		OnAttackSucceeded(AttackData, HitActor, HitResult, bDidShakeCamera);
	}
}

void AAOCharacter::OnAttackSucceeded(const FAttackData& AttackData, AActor* HitActor, const FHitResult& HitResult, bool& bDidShakeCamera)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][OnAttackSucceeded][Enter] Causer=%s HitActor=%s NetMode=%d HasAuthority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(HitActor),
		static_cast<int32>(GetNetMode()),
		HasAuthority() ? 1 : 0
	);

	AAOCharacter* Target = Cast<AAOCharacter>(HitActor);
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][OnAttackSucceeded][Skip] Causer=%s HitActor=%s Reason=TargetCastFailed"), *GetNameSafe(this), *GetNameSafe(HitActor));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][OnAttackSucceeded][CallTakeDamageAO] Causer=%s Target=%s"), *GetNameSafe(this), *GetNameSafe(Target));
	Target->TakeDamageAO(AttackData, HitResult, this);
}

void AAOCharacter::TakeDamageAO(const FAttackData& AttackData, const FHitResult& HitResult, AAOCharacter* DamageCauser)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][TakeDamageAO][Enter] Target=%s Causer=%s NetMode=%d HasAuthority=%d DamageEffect=%s"),
		*GetNameSafe(this),
		*GetNameSafe(DamageCauser),
		static_cast<int32>(GetNetMode()),
		HasAuthority() ? 1 : 0,
		*GetNameSafe(DamageEffect.Get())
	);

	if (!DamageCauser)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][TakeDamageAO][Abort] Target=%s Reason=DamageCauserNull"), *GetNameSafe(this));
		return;
	}

	UAbilitySystemComponent* SourceASC = DamageCauser->GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponent();
	if (!SourceASC || !TargetASC)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[DamageTrace][TakeDamageAO][Abort] Target=%s Causer=%s SourceASC=%s TargetASC=%s"),
			*GetNameSafe(this),
			*GetNameSafe(DamageCauser),
			*GetNameSafe(SourceASC),
			*GetNameSafe(TargetASC)
		);
		return;
	}

	const float AttackPower = SourceASC->GetNumericAttribute(UAOAttributeSet::GetAttackPowerAttribute());
	const float Defense = TargetASC->GetNumericAttribute(UAOAttributeSet::GetDefenseAttribute());

	const float Multiplier = AttackData.DamageMultiplier;
	const float BaseDamage = AttackPower * Multiplier;

	const float FinalDamage = FMath::Max(1.0f, BaseDamage * (100.0f / (100.0f + Defense)));

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][TakeDamageAO][Calc] Causer=%s Target=%s AttackPower=%.1f Defense=%.1f Mult=%.2f Base=%.2f Final=%.2f"),
		*GetNameSafe(DamageCauser),
		*GetNameSafe(this),
		AttackPower,
		Defense,
		Multiplier,
		BaseDamage,
		FinalDamage
	);

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(DamageCauser);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.0f, Context);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][TakeDamageAO][Abort] Target=%s Causer=%s Reason=InvalidDamageSpec DamageEffect=%s"), *GetNameSafe(this), *GetNameSafe(DamageCauser), *GetNameSafe(DamageEffect.Get()));
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(TEXT("Data.Damage")),
		-FinalDamage
	);

	const float OldHealth =
		TargetASC->GetNumericAttribute(
			UAOAttributeSet::GetHealthAttribute()
		);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	if (HasAuthority())
	{
		ForceNetUpdate();

		if (AActor* TargetASCOwner = TargetASC->GetOwnerActor())
		{
			TargetASCOwner->ForceNetUpdate();
		}
	}

	const float NewHealth =
		TargetASC->GetNumericAttribute(
			UAOAttributeSet::GetHealthAttribute()
		);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][TakeDamageAO][Applied] %s -> %s | ATK: %.1f | DEF: %.1f | Mult: %.2f | Final: %.2f | HP: %.1f -> %.1f"),
		*GetNameSafe(DamageCauser),
		*GetNameSafe(this),
		AttackPower,
		Defense,
		Multiplier,
		FinalDamage,
		OldHealth,
		NewHealth
	);

	if (AttackData.HitGameplayCueTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][TakeDamageAO][Cue] Target=%s Cue=%s Impact=%s"), *GetNameSafe(this), *AttackData.HitGameplayCueTag.ToString(), *HitResult.ImpactPoint.ToString());

		FGameplayCueParameters CueParams;
		CueParams.Location = HitResult.ImpactPoint;
		CueParams.Normal = HitResult.ImpactNormal;
		CueParams.Instigator = this;
		CueParams.EffectCauser = this;
		TargetASC->ExecuteGameplayCue(AttackData.HitGameplayCueTag, CueParams);
	}
}

void AAOCharacter::SpawnAttackProjectile(const FAttackData& AttackData, TSubclassOf<class AAOProjectile> ProjectileClass, const FName& SpawnSocket)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][SpawnAttackProjectile][Enter] Causer=%s NetMode=%d HasAuthority=%d ProjectileClass=%s Target=%s Socket=%s"),
		*GetNameSafe(this),
		static_cast<int32>(GetNetMode()),
		HasAuthority() ? 1 : 0,
		*GetNameSafe(ProjectileClass.Get()),
		*GetNameSafe(CurrentTarget),
		*SpawnSocket.ToString()
	);

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][SpawnAttackProjectile][Skip] Causer=%s Reason=NoAuthority"), *GetNameSafe(this));
		return;
	}

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][SpawnAttackProjectile][Abort] Causer=%s Reason=ProjectileClassNull"), *GetNameSafe(this));
		return;
	}

	const FTransform SocketTransform = GetMesh()->GetSocketTransform(SpawnSocket);

	FVector Direction = GetActorForwardVector();
	if (IsValid(CurrentTarget))
	{
		Direction = (CurrentTarget->GetActorLocation() - SocketTransform.GetLocation()).GetSafeNormal();
	}

	FTransform SpawnTransform = SocketTransform;
	SpawnTransform.SetRotation(Direction.Rotation().Quaternion());

	AAOProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAOProjectile>(ProjectileClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][SpawnAttackProjectile][Abort] Causer=%s Reason=SpawnActorDeferredFailed"), *GetNameSafe(this));
		return;
	}

	Projectile->InitProjectile(AttackData, this, CurrentTarget, Direction);
	Projectile->FinishSpawning(SpawnTransform);

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][SpawnAttackProjectile][Spawned] Causer=%s Projectile=%s Target=%s Direction=%s"), *GetNameSafe(this), *GetNameSafe(Projectile), *GetNameSafe(CurrentTarget), *Direction.ToString());
}

bool AAOCharacter::IsEnemy(AActor* TargetActor)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][IsEnemy][Enter] Source=%s TargetActor=%s"),
		*GetNameSafe(this),
		*GetNameSafe(TargetActor)
	);

	AAOCharacter* AOCharacter = Cast<AAOCharacter>(TargetActor);
	if (!AOCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][IsEnemy][Result] Source=%s TargetActor=%s Result=0 Reason=TargetNotAOCharacter"), *GetNameSafe(this), *GetNameSafe(TargetActor));
		return false;
	}

	UAbilitySystemComponent* MyASC = GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = AOCharacter->GetAbilitySystemComponent();
	if (!MyASC || !TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][IsEnemy][Result] Source=%s Target=%s Result=0 MyASC=%s TargetASC=%s"), *GetNameSafe(this), *GetNameSafe(AOCharacter), *GetNameSafe(MyASC), *GetNameSafe(TargetASC));
		return false;
	}

	if (MyASC->HasMatchingGameplayTag(TEAM_DAEVA))
	{
		const bool bResult = TargetASC->HasMatchingGameplayTag(TEAM_MONSTER);
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][IsEnemy][Result] Source=%s SourceTeam=Daeva Target=%s TargetHasMonster=%d Result=%d"), *GetNameSafe(this), *GetNameSafe(AOCharacter), bResult ? 1 : 0, bResult ? 1 : 0);
		return bResult;
	}

	if (MyASC->HasMatchingGameplayTag(TEAM_MONSTER))
	{
		const bool bResult = TargetASC->HasMatchingGameplayTag(TEAM_DAEVA);
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][IsEnemy][Result] Source=%s SourceTeam=Monster Target=%s TargetHasDaeva=%d Result=%d"), *GetNameSafe(this), *GetNameSafe(AOCharacter), bResult ? 1 : 0, bResult ? 1 : 0);
		return bResult;
	}

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][IsEnemy][Result] Source=%s Target=%s Result=0 Reason=SourceHasNoKnownTeamTag"), *GetNameSafe(this), *GetNameSafe(AOCharacter));
	return false;
}

void AAOCharacter::DrawDebugCapsuleCollider(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor)
{
#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 1.0f);
#endif
}

void AAOCharacter::InitGAS()
{
}

void AAOCharacter::ClearGAS()
{
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

TArray<USkeletalMeshComponent*> AAOCharacter::GetAllMeshes()
{
	TArray<USkeletalMeshComponent*> Meshes;
	Meshes.Add(GetMesh());

	return Meshes;
}
