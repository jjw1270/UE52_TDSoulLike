// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/TDSLCharacterBase.h"
#include "GameplayEffectTypes.h"
#include "TDSLGoblin.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API ATDSLGoblin : public ATDSLCharacterBase
{
	GENERATED_BODY()

public:
	ATDSLGoblin(const class FObjectInitializer& ObjectInitializer);

protected:
	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	class UTDSLAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	class UTDSLAttributeSetBase* HardRefAttributeSetBase;

	virtual void BeginPlay() override;

	virtual void Die() override;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDie;

};
