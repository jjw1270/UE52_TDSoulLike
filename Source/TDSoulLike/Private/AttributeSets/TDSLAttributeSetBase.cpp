// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/TDSLAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UTDSLAttributeSetBase::UTDSLAttributeSetBase()
{

}

void UTDSLAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxBlockGageAttribute())
	{
		AdjustAttributeForMaxChange(BlockGage, MaxBlockGage, NewValue, GetBlockGageAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// Cannot slow less than MinMoveSpeed units/s and cannot boost more than MaxMoveSpeed units/s
		NewValue = FMath::Clamp<float>(NewValue, MinMoveSpeed, MaxMoveSpeed);
	}
}

void UTDSLAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// TODO -- Add some logic after Gameplay Effect executed
}

void UTDSLAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, BlockGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, MaxBlockGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, BlockGageRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, Gold, COND_None, REPNOTIFY_Always);

}

void UTDSLAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();

	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilitySystemComponent)
	{
		// Change current value to maintain the Current Value / Maximum Value percentage.
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UTDSLAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, Health, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MaxHealth, OldValue);
}

void UTDSLAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, HealthRegenRate, OldValue);
}

void UTDSLAttributeSetBase::OnRep_BlockGage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, BlockGage, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MaxBlockGage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MaxBlockGage, OldValue);
}

void UTDSLAttributeSetBase::OnRep_BlockGageRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, BlockGageRegenRate, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MoveSpeed, OldValue);
}

void UTDSLAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, Gold, OldValue);
}
