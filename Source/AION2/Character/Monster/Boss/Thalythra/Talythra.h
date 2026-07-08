// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/AOMonsterBase.h"
#include "Types/TalythraTypes.h"
#include "Types/AOTypes.h"
#include "Talythra.generated.h"

UCLASS()
class AION2_API ATalythra : public AAOMonsterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATalythra(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// State Tree에서 몽타주를 실행하기 위해 만든 함수
	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_PlayAttackMontage(class UAnimMontage* MontageToPlay);


#pragma region Projectile
	// 투사체 발사 함수( Animnotify와 같은 프레임에 발사하면 오류발생함 주의!) 
	void FireProjectile();
	// 투사체 발사 전에 타겟 플레이어를 바라보도록 하는 함수
	void TurnToTarget();


	// 회전을 할때 미세한 각도로 인한 떨림 방지 현상을 위한 bool
	void Set_RotationAble(bool RotationOnOff) { RotationAble = RotationOnOff; }
	// 투사체 발사 Line 렌더링 On/Off
	void Set_AttackLineRenderOnOff(bool _OnOff) { AttackLineRenderOnOff = _OnOff; }

	void DoFireProjectile();  // 각 발사 개수마다의 함수 
	void DoFireProjectile_2();
	void DoFireProjectile_3();
#pragma endregion 

#pragma region Charge Attack
	// Charge Attack 관련 
	// 보스 Charge 골반뼈 고정 bool 함수 
	FORCEINLINE void Set_LockPevis(bool _bLock) { bLockPelvis = _bLock; }
	// 보스 Charge는 루트모션이 x -> 직접 이동을 주는 함수
	FORCEINLINE void Set_ChargeAttackMove(bool _bAttack) { bChargeAttack = _bAttack; }
	// 보스 Charge 방향 설정
	FORCEINLINE void Set_ChargeAttackDir(FVector _vector) { ChargeDirection = _vector; }
	void StartChargeMove();
	void EndChargeMove();
#pragma endregion 


#pragma region  Teleport 
	// Teleport 기능 
	void Teleport_To_Player();
	void Teleport_To_Center();
	void Attack_RangeRender(bool _bRenderOnOff);
#pragma endregion 



#pragma region AOE Setting 
	FORCEINLINE void Set_AoeScale(float _fscale) { AttackAoeScale = _fscale; }
	FORCEINLINE void Set_AoeWaringTargetScale(float _fscale) { AttackWarningTargetScale = _fscale; }
	FORCEINLINE void Set_AoeWaringDuartion(float _fDuration) { AttackWarningDuration = _fDuration; }
#pragma endregion 


#pragma region Attribute Setting 
	virtual void InitAttributeSet() override;
#pragma endregion 



#pragma region Wave Pattern

	void SpawnWaveBlue();
	void SpawnWaveRed();

#pragma endregion 

#pragma region SpawnColorOrb Gimmick 


	void SpawnColorOrb();
	void SpawnColorSheid();

	void Set_OrbColorOffset(int _iOffSset) { OrbColorOffset = _iOffSset; }
	void Add_OrbHittedDaeva(class ADaeva* pDaeva);

	void Render_PlayerAoeOnOff(bool _bOnOff);
	void Player_Orb_RenderOnOff(bool _bOnOff);

	void Reset_PlayerOrbStackAndColor();

	FORCEINLINE void Set_OrbAttackColor(EOrbColor _eOrbColor) { AttackOrbColor = _eOrbColor; }
	TArray<class ADaeva*>& Get_ArrayOrbHittedDaeva() { return ArrayOrbHittedDaeva; }
	EOrbColor Get_AttackOrbColor() { return AttackOrbColor; }


	TArray<class ATalythraGimmickShield*>& Get_ArrayOrbShield() { return ArrayOrbShield; }
	void Sub_ArrayOrbShield(class ATalythraGimmickShield* _pSheild);
	void Destroy_OrbShield();

#pragma endregion 





protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 투사체 발사 이펙트. 
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayMuzzleEffect(FVector SpawnLocation, FRotator SpawnRotation);


#pragma region Attack of Area Effect Component ( MutiCast )
	// Attack Line On/Off 기능. 
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackLine_Pattern_1();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackLine_Pattern_1_Off();


	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackLine_Pattern_2();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackLine_Pattern_2_Off();


	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackLine_Pattern_3();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackLine_Pattern_3_Off();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SetChargeMovementParams(bool bChargeMode);


	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackRangeRender(bool _bRendrOnOff);



	// 기믹 관련 HP 설정 
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data) override;



	// EFFECT 관련 
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<class ATalythraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<class ATalythraCollectibleOrb> ProjectileBlueOrbClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<class ATalythraCollectibleOrb> ProjectilePurpleOrbClass;


	// Shield Effect 관련 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GimmickShield")
	TSubclassOf<class ATalythraGimmickShield> ShieldBlueClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GimmickShield")
	TSubclassOf<class ATalythraGimmickShield> ShieldPurpleClass;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ring")
	TSubclassOf<class AWaveCircle> WaveCircleBlueClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ring")
	TSubclassOf<class AWaveCircle> WaveCircleRedClass;




	UPROPERTY(EditAnywhere, Category = "Attack")
	FName ProjectileSocketName = TEXT("WP_Center");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> LanchMuzzleEffect;

#pragma region AOE_Indicator_Line
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeEffect")
	TArray<TObjectPtr<class USceneComponent>> ArrayProjectileLineSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeEffect")
	TObjectPtr<class USceneComponent> AttackRangeSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeEffect")
	TObjectPtr<class USceneComponent> AttackWarningRangeSceneComponent;
#pragma endregion 


	// GAS 관련
protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	TMap<FName, TSubclassOf<class UGameplayAbility>> HasAbilities;


	// 몽타주 관련 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ChargeAttackMontage;



	// 페이즈 및 상태 관련 
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bLockPelvis = false;

private:
	UPROPERTY(VisibleAnywhere)
	int FireCount = 1;

	UPROPERTY(VisibleAnywhere)
	bool RotationAble = false;

	UPROPERTY(VisibleAnywhere)
	bool AttackLineRenderOnOff = false;


	UPROPERTY()
	FVector ChargeDirection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	bool bChargeAttack = false;

	UPROPERTY(VisibleAnywhere)
	bool bMovelAccel = false;


	// AOE 관련 변수 

	UPROPERTY(VisibleAnywhere)
	float AttackAoeScale = 10.f;

	UPROPERTY(VisibleAnywhere)
	float AttackWarningTargetScale = 10.f;

	UPROPERTY(VisibleAnywhere)
	float AttackWarningDuration = 1.3f;

	UPROPERTY(VisibleAnywhere)
	float AttackWarningElapsedTime = 0.0f;


	// 투사체 생성 거리 및 높이 관련 

	UPROPERTY(EditAnywhere, Category = "ProjectileSpawn", meta = (AllowPrivateAccess = "true"))
	float SpawnRadius = 2000.f; // 보스로부터 스폰 거리 
	UPROPERTY(EditAnywhere, Category = "ProjectileSpawn", meta = (AllowPrivateAccess = "true"))
	float SpawnHeight = 60.f; // 지면에서 띄울 높이 

	UPROPERTY(EditAnywhere, Category = "ProjectileSpawn", meta = (AllowPrivateAccess = "true"))
	int8 OrbColorOffset = 0;


	UPROPERTY(EditAnywhere, Category = "TeleportCenter", meta = (AllowPrivateAccess = "true"))
	FVector TeleportCenterLocation;



	// 오브 관련 맞은 플레이어 저장하는 공간 
	TArray<class ADaeva*> ArrayOrbHittedDaeva;
	TArray<class ATalythraGimmickShield*> ArrayOrbShield;

	UPROPERTY(EditAnywhere, Category = "AttackOrbGimmick", meta = (AllowPrivateAccess = "true"))

	EOrbColor AttackOrbColor = EOrbColor::None;

};
