#pragma once


#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_MonsterSpeakSet.generated.h"

class UTexture2D;


UCLASS()
class AION2_API UDA_MonsterSpeakSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Monster | SpeakData")
	int32 MonsterID = -1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Monster | SpeakData")
	FText MonsterName = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, Category = "Monster | SpeakData")
	TObjectPtr<UTexture2D> MonsterImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Monster | SpeakData")
	TMap<int32, FText> SpeakData;
};
