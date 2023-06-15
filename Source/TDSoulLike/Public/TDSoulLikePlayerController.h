// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDSoulLikePlayerController.generated.h"


UCLASS()
class ATDSoulLikePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATDSoulLikePlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

};


