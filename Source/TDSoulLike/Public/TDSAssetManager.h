// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "TDSAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API UTDSAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	virtual void StartInitialLoading() override;
};
