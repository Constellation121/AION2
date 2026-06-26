// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Daeva/Daeva.h"
#include "MMODaeva.generated.h"

/**
 * 
 */
UCLASS()
class AION2_API AMMODaeva : public ADaeva
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

public:
	void SetMyId(uint64 Id) { MyId = Id; }
	void SetMyClass(uint8 InClassType) { MyClassType = InClassType; }

	void SendMovePacket();
	void ReceiveMovePacket(FVector& NewLoc, FRotator& NewRot, FVector& NewVel);

	bool HasMovement();
	bool IsCurrentMoving();

	void SendDungeonWait();

	bool bHasMoveInput = false;

private:
	void InputMoveReleased();

	FTimerHandle SendMoveHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Network")
	float SendMoveTimer = 0.1f;

	FVector LastLoc = FVector::ZeroVector;
	FRotator LastRot = FRotator::ZeroRotator;

	FVector TargetLoc = FVector::ZeroVector;
	FRotator TargetRot = FRotator::ZeroRotator;
	FVector TargetVel = FVector::ZeroVector;

	bool bWasMovingLastSend = false;

	uint64 MyId = -1;
	uint8 MyClassType;
};
