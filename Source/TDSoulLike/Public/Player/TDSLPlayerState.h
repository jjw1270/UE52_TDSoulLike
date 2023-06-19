// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "TDSLPlayerState.generated.h"

UCLASS()
class TDSOULLIKE_API ATDSLPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATDSLPlayerState();

	// Implement the IAbilitySystemInterface. (This is used to find the Ability System Component)
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UTDSLAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	/**
	* Getters for attributes from TDSLAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetBlockGage() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetMaxBlockGage() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetBlockGageRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|PlayerState|Attributes")
	int32 GetGold() const;

protected:
	// The Ability System Component for this PlayerState. 
	UPROPERTY()
	class UTDSLAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UTDSLAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle BlockGageChangedDelegateHandle;
	FDelegateHandle MaxBlockGageChangedDelegateHandle;
	FDelegateHandle BlockGageRegenRateChangedDelegateHandle;
	FDelegateHandle GoldChangedDelegateHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void BlockGageChanged(const FOnAttributeChangeData& Data);
	virtual void MaxBlockGageChanged(const FOnAttributeChangeData& Data);
	virtual void BlockGageRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void GoldChanged(const FOnAttributeChangeData& Data);
};
