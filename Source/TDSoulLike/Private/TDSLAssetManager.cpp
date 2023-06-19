// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSLAssetManager.h"
#include "AbilitySystemGlobals.h"

void UTDSLAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
