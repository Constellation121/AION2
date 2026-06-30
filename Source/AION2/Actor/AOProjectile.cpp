#include "Actor/AOProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/AOCharacter.h"

AAOProjectile::AAOProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);

    Collision->InitSphereRadius(20.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
}

void AAOProjectile::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[DamageTrace][AOProjectile][BeginPlay] Projectile=%s NetMode=%d HasAuthority=%d DamageCauser=%s Target=%s"),
        *GetNameSafe(this),
        static_cast<int32>(GetNetMode()),
        HasAuthority() ? 1 : 0,
        *GetNameSafe(DamageCauser),
        *GetNameSafe(Target)
    );

    SetLifeSpan(LifeSeconds);

    ProjectileMovement->InitialSpeed = ProjectileSpeed;
    ProjectileMovement->MaxSpeed = ProjectileSpeed;

    if (HasAuthority())
    {
        Collision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileOverlap);
        UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][BeginPlay] Projectile=%s BoundOverlap=1"), *GetNameSafe(this));
    }
}

void AAOProjectile::InitProjectile(const FAttackData& InAttackData, AAOCharacter* InDamageCauser, AAOCharacter* InTarget, const FVector& InDirection)
{
    AttackData = InAttackData;
    DamageCauser = InDamageCauser;
    Target = InTarget;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[DamageTrace][AOProjectile][Init] Projectile=%s NetMode=%d HasAuthority=%d DamageCauser=%s Target=%s Direction=%s"),
        *GetNameSafe(this),
        static_cast<int32>(GetNetMode()),
        HasAuthority() ? 1 : 0,
        *GetNameSafe(DamageCauser),
        *GetNameSafe(Target),
        *InDirection.ToString()
    );

    const FVector Direction = InDirection.GetSafeNormal();

    ProjectileMovement->InitialSpeed = ProjectileSpeed;
    ProjectileMovement->MaxSpeed = ProjectileSpeed;
    ProjectileMovement->Velocity = Direction * ProjectileSpeed;

    if (bHoming && IsValid(Target))
    {
        ProjectileMovement->bIsHomingProjectile = true;
        ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
        ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;
    }
    else
    {
        ProjectileMovement->bIsHomingProjectile = false;
    }
}

void AAOProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[DamageTrace][AOProjectile][Overlap] Projectile=%s NetMode=%d HasAuthority=%d Other=%s OtherComp=%s Target=%s Causer=%s"),
        *GetNameSafe(this),
        static_cast<int32>(GetNetMode()),
        HasAuthority() ? 1 : 0,
        *GetNameSafe(OtherActor),
        *GetNameSafe(OtherComp),
        *GetNameSafe(Target),
        *GetNameSafe(DamageCauser)
    );

    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Overlap][Skip] Projectile=%s Reason=NoAuthority"), *GetNameSafe(this));
        return;
    }

    AAOCharacter* HitCharacter = Cast<AAOCharacter>(OtherActor);
    if (!HitCharacter || HitCharacter != Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Overlap][Reject] Projectile=%s Other=%s HitCharacter=%s Target=%s Reason=NotTarget"), *GetNameSafe(this), *GetNameSafe(OtherActor), *GetNameSafe(HitCharacter), *GetNameSafe(Target));
        return;
    }

    if (HitCharacter->IsDead())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Overlap][Reject] Projectile=%s Target=%s Reason=TargetDead"), *GetNameSafe(this), *GetNameSafe(HitCharacter));
        return;
    }

    if (!DamageCauser)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Overlap][Abort] Projectile=%s Target=%s Reason=DamageCauserNull"), *GetNameSafe(this), *GetNameSafe(HitCharacter));
        return;
    }

    if (!DamageCauser->IsEnemy(HitCharacter))
    {
        UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Overlap][Reject] Projectile=%s Causer=%s Target=%s Reason=NotEnemy"), *GetNameSafe(this), *GetNameSafe(DamageCauser), *GetNameSafe(HitCharacter));
        return;
    }

    bool bDidCameraShake = false;
    UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Overlap][Hit] Projectile=%s Causer=%s Target=%s"), *GetNameSafe(this), *GetNameSafe(DamageCauser), *GetNameSafe(Target));
    DamageCauser->OnAttackSucceeded(AttackData, Target, SweepResult, bDidCameraShake);

    UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AOProjectile][Destroy] Projectile=%s"), *GetNameSafe(this));
    Destroy();
}
