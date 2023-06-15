// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TDSAttributeSet.h"
#include "Net/Unrealnetwork.h"
#include "GameplayEffectExtension.h"

UTDSAttributeSet::UTDSAttributeSet()
	: Health(40.0f), MaxHealth(60.0f), Armor(50.0f), MaxArmor(50.0f)
{
}


void UTDSAttributeSet::ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxArmor());
	}
}

void UTDSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetInDamageAttribute())
	{
		float InDamageDone = GetInDamage();
		SetInDamage(0.0f);
		if (InDamageDone > 0.0f)
		{
			const float NewArmor = GetArmor() - InDamageDone;
			SetArmor(FMath::Clamp(NewArmor, 0.0f, GetMaxArmor()));
			if ((GetArmor() <= 0.0f) && !bOutOfArmor)
			{
				if (OnOutOfArmor.IsBound())
				{
					const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
					AActor* Instigator = EffectContext.GetOriginalInstigator();
					AActor* Causer = EffectContext.GetEffectCauser();

					OnOutOfArmor.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
				}
			}
			bOutOfArmor = (GetArmor() <= 0.0f);

			float NewHealth = GetHealth();
			if (NewArmor < 0.0f)
			{
				NewHealth += NewArmor;
			}

			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			if ((GetHealth() <= 0.0f) && !bOutOfHealth)
			{
				if (OnOutOfHealth.IsBound())
				{
					const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
					AActor* Instigator = EffectContext.GetOriginalInstigator();
					AActor* Causer = EffectContext.GetEffectCauser();

					OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
				}
			}
		}
	}
}

void UTDSAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttributeOnChange(Attribute, NewValue);
}

void UTDSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttributeOnChange(Attribute, NewValue);
}


void UTDSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, MaxArmor, COND_None, REPNOTIFY_Always);
}

void UTDSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, Health, OldHealth);
}

void UTDSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, MaxHealth, OldMaxHealth);
}

void UTDSAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, Armor, OldArmor);
}

void UTDSAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, MaxArmor, OldMaxArmor);
}
