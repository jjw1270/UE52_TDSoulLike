// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AbilitySystemGlobalsSubsystem.generated.h"

// Initializes the UAbilitySystemGlobals module.
// "UAbilitySystemGlobals::Get().InitGlobalData()" needs to be called to enable Ability Target Data.
// This Subsystem will automatically startup when the main game module is launched.
// Note: You could call this in other places. It is added here in the example for visibility.

UCLASS()
class TDSOULLIKE_API UAbilitySystemGlobalsSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};
