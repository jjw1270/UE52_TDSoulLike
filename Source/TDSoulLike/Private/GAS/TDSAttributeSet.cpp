// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TDSAttributeSet.h"
#include "Net/Unrealnetwork.h"
#include "GameplayEffectExtension.h"

UTDSAttributeSet::UTDSAttributeSet()
	: Health(40.0f), MaxHealth(60.0f), Armor(50.0f), MaxArmor(50.0f), CritChance(0.0f), CritMulti(1.5f), LuckyChance(5.0f),
	DamageAdd(0.0f), DamageMulti(1.0f), Ammo(100.0f), Chilled(0.0f), DeChill(10.0f)
{
}


void UTDSAttributeSet::ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxArmor());
	}
	else if (Attribute == GetChilledAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
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
			if (GetArmor() > 0.0f)
			{
				float InDamageDoneToArmor = InDamageDone;
				const FGameplayTag AcidDamageTag = FGameplayTag::RequestGameplayTag(FName("Damage.Type.Acid"), false);
				bool IsAcidDamage = Data.EffectSpec.CapturedSourceTags.GetSpecTags().HasTagExact(AcidDamageTag);
				if (IsAcidDamage)
				{
					InDamageDoneToArmor *= 1.5f;
				}
				float NewArmor = GetArmor();
				const float ArmorDiff = FMath::Min(NewArmor, InDamageDoneToArmor);
				InDamageDone -= ArmorDiff;
				NewArmor -= ArmorDiff;
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
			}

			if (InDamageDone > 0.0f)
			{
				float InDamageDoneToHealth = InDamageDone;
				const FGameplayTag FireDamageTag = FGameplayTag::RequestGameplayTag(FName("Damage.Type.Fire"), false);
				bool IsFireDamage = Data.EffectSpec.CapturedSourceTags.GetSpecTags().HasTagExact(FireDamageTag);
				if (IsFireDamage)
				{
					InDamageDoneToHealth *= 1.5f;
				}
				const float NewHealth = GetHealth() - InDamageDoneToHealth;

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
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, CritMulti, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, LuckyChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, DamageAdd, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, DamageMulti, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, Chilled, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSAttributeSet, DeChill, COND_None, REPNOTIFY_Always);
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

void UTDSAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldCritChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, CritChance, OldCritChance);
}

void UTDSAttributeSet::OnRep_CritMulti(const FGameplayAttributeData& OldCritMulti)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, CritMulti, OldCritMulti);
}

void UTDSAttributeSet::OnRep_LuckyChance(const FGameplayAttributeData& OldLuckyChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, LuckyChance, OldLuckyChance);
}

void UTDSAttributeSet::OnRep_DamageAdd(const FGameplayAttributeData& OldDamageAdd)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, DamageAdd, OldDamageAdd);
}

void UTDSAttributeSet::OnRep_DamageMulti(const FGameplayAttributeData& OldDamageMulti)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, DamageMulti, OldDamageMulti);
}

void UTDSAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, Ammo, OldAmmo);
}

void UTDSAttributeSet::OnRep_Chilled(const FGameplayAttributeData& OldChilled)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, Chilled, OldChilled);
}

void UTDSAttributeSet::OnRep_DeChill(const FGameplayAttributeData& OldDeChill)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSAttributeSet, DeChill, OldDeChill);
}
