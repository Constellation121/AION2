// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOPlayerHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/AOAttributeSet.h"
#include "Player/AOPlayerState.h"


void UAOPlayerHUDWidget::BindToPlayerState(AAOPlayerState* InPlayerState)
{
    UnbindASCDelegates();

    Super::BindToPlayerState(InPlayerState);

    if (!BoundASC)
    {
        return;
    }

    BindASCDelegates();

    // Bind된 delegate들이 변경될 때만 호출되므로, 초기값 Push.
    BroadcastInitialAttributes();
}

void UAOPlayerHUDWidget::NativeDestruct()
{
    UnbindASCDelegates();
    Super::NativeDestruct();
}

void UAOPlayerHUDWidget::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnHealthChanged(Data.NewValue, AttributeSet->GetMaxHealth());
}

void UAOPlayerHUDWidget::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnMaxHealthChanged(AttributeSet->GetHealth(), Data.NewValue);
}

void UAOPlayerHUDWidget::HandleManaChanged(const FOnAttributeChangeData& Data)
{
    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnManaChanged(Data.NewValue, AttributeSet->GetMaxMana());
}

void UAOPlayerHUDWidget::HandleMaxManaChanged(const FOnAttributeChangeData& Data)
{
    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnMaxManaChanged(AttributeSet->GetMana(), Data.NewValue);
}

void UAOPlayerHUDWidget::HandleStaminaChanged(const FOnAttributeChangeData& Data)
{
    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnStaminaChanged(Data.NewValue, AttributeSet->GetMaxStamina());
}

void UAOPlayerHUDWidget::HandleMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnMaxStaminaChanged(AttributeSet->GetStamina(), Data.NewValue);
}

void UAOPlayerHUDWidget::BindASCDelegates()
{
    // Health Bind
    HealthChangedHandle = BoundASC->GetGameplayAttributeValueChangeDelegate(
        UAOAttributeSet::GetHealthAttribute()
    ).AddUObject(this, &UAOPlayerHUDWidget::HandleHealthChanged);

    MaxHealthChangedHandle = BoundASC->GetGameplayAttributeValueChangeDelegate(
        UAOAttributeSet::GetMaxHealthAttribute()
    ).AddUObject(this, &UAOPlayerHUDWidget::HandleMaxHealthChanged);

    // Mana Bind
    ManaChangedHandle = BoundASC->GetGameplayAttributeValueChangeDelegate(
        UAOAttributeSet::GetManaAttribute()
    ).AddUObject(this, &UAOPlayerHUDWidget::HandleManaChanged);

    MaxManaChangedHandle = BoundASC->GetGameplayAttributeValueChangeDelegate(
        UAOAttributeSet::GetMaxManaAttribute()
    ).AddUObject(this, &UAOPlayerHUDWidget::HandleMaxManaChanged);

    // Stamina Bind
    StaminaChangedHandle = BoundASC->GetGameplayAttributeValueChangeDelegate(
        UAOAttributeSet::GetStaminaAttribute()
    ).AddUObject(this, &UAOPlayerHUDWidget::HandleStaminaChanged);

    MaxStaminaChangedHandle = BoundASC->GetGameplayAttributeValueChangeDelegate(
        UAOAttributeSet::GetMaxStaminaAttribute()
    ).AddUObject(this, &UAOPlayerHUDWidget::HandleMaxStaminaChanged);
}

void UAOPlayerHUDWidget::UnbindASCDelegates()
{
    if (!BoundASC)
    {
        return;
    }

    // Health 해제
    if (HealthChangedHandle.IsValid())
    {
        BoundASC->GetGameplayAttributeValueChangeDelegate(
            UAOAttributeSet::GetHealthAttribute()
        ).Remove(HealthChangedHandle);

        HealthChangedHandle.Reset();
    }

    if (MaxHealthChangedHandle.IsValid())
    {
        BoundASC->GetGameplayAttributeValueChangeDelegate(
            UAOAttributeSet::GetMaxHealthAttribute()
        ).Remove(MaxHealthChangedHandle);

        MaxHealthChangedHandle.Reset();
    }

    // Mana 해제
    if (ManaChangedHandle.IsValid())
    {
        BoundASC->GetGameplayAttributeValueChangeDelegate(
            UAOAttributeSet::GetManaAttribute()
        ).Remove(ManaChangedHandle);

        ManaChangedHandle.Reset();
    }

    if (MaxManaChangedHandle.IsValid())
    {
        BoundASC->GetGameplayAttributeValueChangeDelegate(
            UAOAttributeSet::GetMaxManaAttribute()
        ).Remove(MaxManaChangedHandle);

        MaxManaChangedHandle.Reset();
    }

    // Stamina 해제
    if (StaminaChangedHandle.IsValid())
    {
        BoundASC->GetGameplayAttributeValueChangeDelegate(
            UAOAttributeSet::GetStaminaAttribute()
        ).Remove(StaminaChangedHandle);

        StaminaChangedHandle.Reset();
    }

    if (MaxStaminaChangedHandle.IsValid())
    {
        BoundASC->GetGameplayAttributeValueChangeDelegate(
            UAOAttributeSet::GetMaxStaminaAttribute()
        ).Remove(MaxStaminaChangedHandle);

        MaxStaminaChangedHandle.Reset();
    }
}

void UAOPlayerHUDWidget::BroadcastInitialAttributes()
{
    if (!BoundASC)
    {
        return;
    }

    const UAOAttributeSet* AttributeSet = BoundASC->GetSet<UAOAttributeSet>();
    if (!AttributeSet)
    {
        return;
    }

    OnHealthChanged(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
    OnManaChanged(AttributeSet->GetMana(), AttributeSet->GetMaxMana());
    OnStaminaChanged(AttributeSet->GetStamina(), AttributeSet->GetMaxStamina());
}
