// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mail/MainMailWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "MailListRowWidget.h"
#include "Components/ScrollBox.h"
#include "Network/PacketHeader.h"
#include "Manager/AOUIManager.h"
#include "AOQuickSlotComponent.h"
#include  "AION2.h"


void UMainMailWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (MailSwitcher)
	{
		MailSwitcher->SetActiveWidgetIndex(0);
	}
	if (ScrollBox)
	{
		ScrollBox->ClearChildren();
	}

	if (DetailGold)
	{
		DetailGold->OnClicked.AddUniqueDynamic(this, &UMainMailWidget::OnGoldClicked);
	}
	if (DetailItem)
	{
		DetailItem->OnClicked.AddUniqueDynamic(this, &UMainMailWidget::OnItemClicked);
	}
}

bool UMainMailWidget::HandleEscapeKey()
{
	if (!MailSwitcher) return false;

	int32 ActiveIndex = MailSwitcher->GetActiveWidgetIndex();
	
	if (ActiveIndex == 1)
	{
		MailSwitcher->SetActiveWidgetIndex(0);
		return true; 
	}
	
	UAOUIManager* UIManager = GetWorld() ? GetWorld()->GetGameInstance()->GetSubsystem<UAOUIManager>() : nullptr;
	if (UIManager)
	{
		UIManager->HideWidget(this);
	}
	else
	{
		RemoveFromParent();
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	return true;
}

void UMainMailWidget::ShowMailList()
{
	if (MailSwitcher)
	{
		MailSwitcher->SetActiveWidgetIndex(0);
	}
}

void UMainMailWidget::UpdateMailList(const TArray<FMailData>& InMailList)
{
	MailListCache = InMailList;
	if (!ScrollBox || !RowWidgetClass) return;

	ScrollBox->ClearChildren();

	for (const FMailData& Data : MailListCache)
	{
		UMailListRowWidget* NewRow = CreateWidget<UMailListRowWidget>(this, RowWidgetClass);
		if (NewRow)
		{
			NewRow->InitRowData(Data, this);
			ScrollBox->AddChild(NewRow);
		}
	}
}

void UMainMailWidget::UpdateMailContent(const FMailData& InDetailData)
{
	CurrentMailData = InDetailData;

	// 캐시 데이터 업데이트
	for (FMailData& Data : MailListCache)
	{
		if (Data.MailUID == InDetailData.MailUID)
		{
			Data = InDetailData;
			break;
		}
	}
	
	// 상세 UI 컴포넌트 갱신
	if (DetailTitle)
	{
		DetailTitle->SetText(FText::FromString(InDetailData.Title));
	}

	if (DetailContent)
	{
		DetailContent->SetText(FText::FromString(InDetailData.Content));
	}

	if (DetailSender)
	{
		DetailSender->SetText(FText::FromString(InDetailData.SenderName));
	}

	// 골드 표시 제어
	if (DetailGold || GoldSection)
	{
		if (InDetailData.Gold > 0)
		{
			if (DetailGoldText)
			{
				DetailGoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), InDetailData.Gold)));
			}
			
			if (GoldSection)
			{
				GoldSection->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else if (DetailGold)
			{
				DetailGold->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (GoldSection)
			{
				GoldSection->SetVisibility(ESlateVisibility::Collapsed);
			}
			else if (DetailGold)
			{
				DetailGold->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	// 아이템 표시 제어
	if (DetailItem || ItemSection)
	{
		if (InDetailData.ItemId > 0 && InDetailData.ItemCount > 0)
		{
			FString DispItemName = FString::Printf(TEXT("Item: %d"), InDetailData.ItemId);
			UTexture2D* IconTexture = nullptr;

			if (APlayerController* PC = GetOwningPlayer())
			{
				if (APawn* Pawn = PC->GetPawn())
				{
					UAOQuickSlotComponent* QuickSlotComp = Pawn->FindComponentByClass<UAOQuickSlotComponent>();
					if (QuickSlotComp)
					{
						FItemData ItemTemplateData;
						if (QuickSlotComp->FindItemTemplateData(InDetailData.ItemId, ItemTemplateData))
						{
							DispItemName = ItemTemplateData.ItemName.ToString();
							IconTexture = ItemTemplateData.ItemIcon;
						}
					}
				}
			}

			if (IconTexture)
			{
				if (DetailItem)
				{
					FButtonStyle Style = DetailItem->GetStyle();
					Style.Normal.SetResourceObject(IconTexture);
					Style.Hovered.SetResourceObject(IconTexture);
					Style.Pressed.SetResourceObject(IconTexture);
					DetailItem->SetStyle(Style);
				}
			}

			if (DetailItemCount)
			{
				DetailItemCount->SetText(FText::FromString(FString::Printf(TEXT("x%d"), InDetailData.ItemCount)));
				DetailItemCount->SetVisibility(ESlateVisibility::Visible);
			}
			
			if (ItemSection)
			{
				ItemSection->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else if (DetailItem)
			{
				DetailItem->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (DetailItemCount)
			{
				DetailItemCount->SetVisibility(ESlateVisibility::Collapsed);
			}
			if (ItemSection)
			{
				ItemSection->SetVisibility(ESlateVisibility::Collapsed);
			}
			else if (DetailItem)
			{
				DetailItem->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (MailSwitcher)
	{
		MailSwitcher->SetActiveWidgetIndex(1);
	}
}

void UMainMailWidget::RequestDetailContentFromServer(int64 MailUID)
{
	Protocol::C_MailContentPacket ReqContent;
	ReqContent.set_mailid(MailUID);
	SEND_PACKET(ReqContent, PKT_C_MAILCONTENT);
}

void UMainMailWidget::OnGoldClicked()
{
	OnClaimGold(CurrentMailData.Gold);
}

void UMainMailWidget::OnItemClicked()
{
	OnClaimItem(CurrentMailData.ItemId, CurrentMailData.ItemCount);
}
