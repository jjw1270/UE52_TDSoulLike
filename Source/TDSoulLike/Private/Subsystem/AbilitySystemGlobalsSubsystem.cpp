// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/AbilitySystemGlobalsSubsystem.h"
#include "AbilitySystemGlobals.h"

void UAbilitySystemGlobalsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}
