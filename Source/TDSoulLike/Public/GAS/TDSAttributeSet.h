// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TDSAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(FGGAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitute*/);

UCLASS()
class TDSOULLIKE_API UTDSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
    UTDSAttributeSet();

    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const;

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, Health);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, MaxHealth);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, Armor);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MaxArmor;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, MaxArmor);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData InDamage;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, InDamage);  //doesn't need OnRep_ because it is Instant attribute

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritChance, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData CritChance;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, CritChance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritMulti, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData CritMulti;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, CritMulti);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LuckyChance, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData LuckyChance;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, LuckyChance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageAdd, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData DamageAdd;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, DamageAdd);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageMulti, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData DamageMulti;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, DamageMulti);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ammo, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Ammo;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, Ammo);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Chilled, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Chilled;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, Chilled);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DeChill, Category = "Attributes", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData DeChill;
    ATTRIBUTE_ACCESSORS(UTDSAttributeSet, DeChill);

    mutable FGGAttributeEvent OnOutOfHealth;
    mutable FGGAttributeEvent OnOutOfArmor;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

    UFUNCTION()
    virtual void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor);

    UFUNCTION()
    virtual void OnRep_CritChance(const FGameplayAttributeData& OldCritChance);

    UFUNCTION()
    virtual void OnRep_CritMulti(const FGameplayAttributeData& OldCritMulti);

    UFUNCTION()
    virtual void OnRep_LuckyChance(const FGameplayAttributeData& OldLuckyChance);

    UFUNCTION()
    virtual void OnRep_DamageAdd(const FGameplayAttributeData& OldDamageAdd);

    UFUNCTION()
    virtual void OnRep_DamageMulti(const FGameplayAttributeData& OldDamageMulti);

    UFUNCTION()
    virtual void OnRep_Ammo(const FGameplayAttributeData& OldAmmo);

    UFUNCTION()
    virtual void OnRep_Chilled(const FGameplayAttributeData& OldChilled);

    UFUNCTION()
    virtual void OnRep_DeChill(const FGameplayAttributeData& OldDeChill);

    bool bOutOfHealth;
    bool bOutOfArmor;
};
