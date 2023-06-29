// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDSLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API ATDSLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATDSLPlayerController();

	void CreateHUD();
	class UTDSLHUDWidget* GetHUD();

	void ShowEnemyInfoHUD(class ATDSLCharacterBase* TargetCharacter);
	void HideEnemyInfoHUD();
	class UTDSLEnemyInfoWidget* GetEnemyHUD();
private:
	FTimerHandle TimerHandle_HideEnemyInfoHUD;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS|UI")
	TSubclassOf<class UTDSLHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GAS|UI")
	class UTDSLHUDWidget* UIHUDWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS|UI")
	TSubclassOf<class UTDSLEnemyInfoWidget> UIEnemyInfoWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GAS|UI")
	class UTDSLEnemyInfoWidget* UIEnemyInfoWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;
};
