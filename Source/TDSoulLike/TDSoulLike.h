// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETDSLHitReactDirection : uint8
{
	// 0
	None			UMETA(DisplayName = "None"),
	// 1
	Front 			UMETA(DisplayName = "Front"),
	// 2
	Back			UMETA(DisplayName = "Back")
};

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

	StandardAtk			UMETA(DisplayName = "StandardAtk"),

	PowerAtk			UMETA(DisplayName = "PowerAtk"),

	SpecialAtk			UMETA(DisplayName = "SpecialAtk"),

	Ability1			UMETA(DisplayName = "Ability1"),

	SwitchPose			UMETA(DisplayName = "SwitchPose"),

	Move				UMETA(DisplayName = "Move"),

	Sprint				UMETA(DisplayName = "Sprint"),

	Target				UMETA(DisplayName = "Target"),

	Block				UMETA(DisplayName = "Block"),

	Roll				UMETA(DisplayName = "Roll"),
};