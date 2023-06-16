// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "TDSGameplayEffectDamageCalc.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API UTDSGameplayEffectDamageCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UTDSGameplayEffectDamageCalc();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
