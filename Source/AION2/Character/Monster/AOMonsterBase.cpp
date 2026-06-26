// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/AOMonsterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "GAS/AOGameplayTags.h"
#include "Data/DA_AbilitySet.h"
#include "GAS/AttributeSet/AOAttributeSet.h"

// Sets default values
AAOMonsterBase::AAOMonsterBase(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.


    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    ASC->SetIsReplicated(true);
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


    AttributeSet = CreateDefaultSubobject<UAOAttributeSet>(TEXT("AttributeSet"));


    bReplicates = true;
    SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AAOMonsterBase::BeginPlay()
{
    Super::BeginPlay();

}

void AAOMonsterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);


    InitGAS();



}

void AAOMonsterBase::InitGAS()
{
    // Owner Actor란? : ASC를 논리적으로 소유한 주체 ( 해당 주체가 죽어도 유지되며, 레벨 및 스텟 보존 )
    // Avatar Actor란? : Character  (죽으면 바뀌는 물체, 실제로 데이터를 처리하지 않지만 비주얼만 수행해주는 엑터) 
    ASC->InitAbilityActorInfo(this, this);

    if (!ASC->HasMatchingGameplayTag(TEAM_MONSTER))
    {
        ASC->AddLooseGameplayTag(TEAM_MONSTER);
    }


    if (HasAuthority() == false)
        return;

    // 서버 로직 

    // ASC에 능력 넣어주는 작업 
    AbilitySet->GiveToASC(ASC, AbilityHandles);


}

void AAOMonsterBase::ClearGAS()
{
}


// Called every frame
void AAOMonsterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

UAnimMontage* AAOMonsterBase::GetMontageByTag(const FGameplayTag& MontageTag) const
{
    if (const TObjectPtr<UAnimMontage>* FoundMontage = MontageMap.Find(MontageTag))
    {
        return *FoundMontage;
    }

    return nullptr;
}

void AAOMonsterBase::InitAttributeSet()
{


}



bool AAOMonsterBase::CanMoveOnNavMesh(const FVector Direction, float Distance)
{
    const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

    if (!NavSys)
        return false;

    FVector Dir2D = Direction;
    Dir2D.Z = 0.f;

    if (!Dir2D.Normalize())
        return false;

    // Start를 발바닥 높이로 수정  
    const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    FVector Start = GetActorLocation();
    Start.Z -= HalfHeight;

    const FVector End = Start + Dir2D * Distance;

    FVector HitLocation;
    const bool bHit = NavSys->NavigationRaycast(GetWorld(), Start, End, HitLocation);


    return !bHit;
}