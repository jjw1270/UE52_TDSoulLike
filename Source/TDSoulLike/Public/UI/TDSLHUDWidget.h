// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDSLHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API UTDSLHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


	/**
	* Attribute setters
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentHealth(float CurrentHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthRegenRate(float HealthRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxStamina(float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentStamina(float CurrentStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetStaminaPercentage(float StaminaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetStaminaRegenRate(float StaminaRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxBlockGage(float MaxBlockGage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentBlockGage(float CurrentBlockGage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetBlockGagePercentage(float BlockGagePercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetBlockGageRegenRate(float BlockGageRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetGold(int32 Gold);

};
