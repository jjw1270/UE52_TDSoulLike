// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// TDSLAbilityInputID provide the ability to bind GA's to input bindings in Editor.
// These bindings will be provided as an Enum on the "TDSLGameplayAbility" class and can be
// bound in editor under Project Settings->Engine->Input->Action Mappings. For passive Abilities
// or Abilities that do not use input, set the binding to "None".

UENUM(BlueprintType)
enum class ETDSLAbilityInputID : uint8
{
	None				UMETA(DisplayName = "None"),

	Confirm				UMETA(DisplayName = "Confirm"),

	Cancel				UMETA(DisplayName = "Cancel"),

	Ability1			UMETA(DisplayName = "Ability1"),

	Ability2			UMETA(DisplayName = "Ability2"),

	Ability3			UMETA(DisplayName = "Ability3"),

	Ability4			UMETA(DisplayName = "Ability4"),

	Ability5			UMETA(DisplayName = "Ability5"),

	Move				UMETA(DisplayName = "Move"),

	Target				UMETA(DisplayName = "Target"),

	Block				UMETA(DisplayName = "Block"),

	Dash				UMETA(DisplayName = "Dash"),
};