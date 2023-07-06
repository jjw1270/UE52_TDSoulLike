// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Runtime/Core/Public/UObject/NameTypes.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"

namespace BBKeys
{
	TCHAR const* const TargetActor = TEXT("TargetActor");
	TCHAR const* const AIState = TEXT("AIState");
}

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),

	Chase 			UMETA(DisplayName = "Chase"),

	Battle			UMETA(DisplayName = "Battle")
};
