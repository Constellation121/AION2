// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MailData.h"
#include "MainMailWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UButton;
class UImage;
class UAOClassSwitcherWidget;
class UMailListRowWidget;

UCLASS()
class AION2_API UMainMailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MailSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox;
	

	
	UPROPERTY(EditDefaultsOnly, Category = "MainUI")
	TSubclassOf<class UMailListRowWidget> RowWidgetClass;

	// 상세 정보 표시용 위젯들 (Optional 바인딩으로 컴파일 유연성 확보)
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DetailTitle;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DetailContent;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DetailSender;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* DetailGold;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DetailGoldText;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* DetailItem;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DetailItemText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DetailItemCount;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* DetailItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* GoldSection;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* ItemSection;
	
private:
	TArray<struct FMailData> MailListCache;
	FMailData CurrentMailData;
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnGoldClicked();

	UFUNCTION()
	void OnItemClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mail UI")
	void OnClaimGold(int32 GoldAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mail UI")
	void OnClaimItem(int32 ItemId, int32 ItemCount);
	
public:
	// UI 매니저 연동 ESC 제어 함수
    virtual bool HandleEscapeKey();
    
    UFUNCTION(BlueprintCallable, Category = "Mail UI")
    void ShowMailList();

    // 네트워크 매니저 패킷 수신 콜백 연동용 함수들
    void UpdateMailList(const TArray<FMailData>& InMailList);
    void UpdateMailContent(const FMailData& InDetailData);
    
    // 하위 Row에서 호출할 서버 요청 전스용 함수
    void RequestDetailContentFromServer(int64 MailUID);	
};
