// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "TDSLAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API UTDSLAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	virtual void StartInitialLoading() override;

};
