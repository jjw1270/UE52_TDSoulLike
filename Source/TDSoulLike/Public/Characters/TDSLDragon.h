// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/TDSLCharacterBase.h"
#include "GameplayEffectTypes.h"
#include "TDSLDragon.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API ATDSLDragon : public ATDSLCharacterBase
{
	GENERATED_BODY()
	
public:
	ATDSLDragon(const class FObjectInitializer& ObjectInitializer);

protected:
	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	class UTDSLAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	class UTDSLAttributeSetBase* HardRefAttributeSetBase;

	virtual void BeginPlay() override;

	virtual void Die() override;
};
