// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSAssetManager.h"
#include "AbilitySystemGlobals.h"

void UTDSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
