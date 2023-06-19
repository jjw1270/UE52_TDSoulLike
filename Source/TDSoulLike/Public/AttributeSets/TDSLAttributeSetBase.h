// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TDSLAttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TDSOULLIKE_API UTDSLAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UTDSLAttributeSetBase();

	// Attribute Set Overrides.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, Damage)

	// Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth.
	// Positive changes can directly use this.
	// Negative changes to Health should go through Damage meta attribute.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, Health)

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, MaxHealth)

	// Health regen rate will passively increase Health every second
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, HealthRegenRate)

	// BlockGage is used to execute block ability. Capped by MaxBlockGage
	UPROPERTY(BlueprintReadOnly, Category = "BlockGage", ReplicatedUsing = OnRep_BlockGage)
	FGameplayAttributeData BlockGage;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, BlockGage)

	// Holds the value for Maximum BlockGage.
	UPROPERTY(BlueprintReadOnly, Category = "BlockGage", ReplicatedUsing = OnRep_MaxBlockGage)
	FGameplayAttributeData MaxBlockGage;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, MaxBlockGage)

	// Holds the current value for BlockGage Regeneration.
	UPROPERTY(BlueprintReadOnly, Category = "BlockGage", ReplicatedUsing = OnRep_BlockGageRegenRate)
	FGameplayAttributeData BlockGageRegenRate;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, BlockGageRegenRate)

	// MoveSpeed affects how fast characters can move.
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, MoveSpeed)

	UPROPERTY(EditDefaultsOnly, Category = "MoveSpeed")
	float MinMoveSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "MoveSpeed")
	float MaxMoveSpeed;

	// Gold gained from killing enemies. Used to purchase items
	UPROPERTY(BlueprintReadOnly, Category = "Gold", ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UTDSLAttributeSetBase, Gold)

protected:
	// Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	// (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	/**
	* These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	**/

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BlockGage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxBlockGage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BlockGageRegenRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Gold(const FGameplayAttributeData& OldValue);


};
