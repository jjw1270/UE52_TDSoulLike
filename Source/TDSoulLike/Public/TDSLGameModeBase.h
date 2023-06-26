// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TDSLGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API ATDSLGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATDSLGameModeBase();

	void PlayerDied(AController* Controller);
	
};
