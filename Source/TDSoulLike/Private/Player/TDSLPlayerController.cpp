// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TDSLPlayerController.h"
#include "TDSLAbilitySystemComponent.h"
#include "Player/TDSLPlayerState.h"
#include "UI/TDSLHUDWidget.h"
#include "UI/TDSLEnemyInfoWidget.h"
#include "Characters/TDSLCharacterBase.h"

ATDSLPlayerController::ATDSLPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATDSLPlayerController::CreateHUD()
{	
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	ATDSLPlayerState* PS = GetPlayerState<ATDSLPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UTDSLHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / FMath::Max<float>(PS->GetMaxHealth(), 1.f));
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / FMath::Max<float>(PS->GetMaxStamina(), 1.f));
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
	UIHUDWidget->SetCurrentBlockGage(PS->GetBlockGage());
	UIHUDWidget->SetMaxBlockGage(PS->GetMaxBlockGage());
	UIHUDWidget->SetBlockGagePercentage(PS->GetBlockGage() / FMath::Max<float>(PS->GetMaxBlockGage(), 1.f));
	UIHUDWidget->SetBlockGageRegenRate(PS->GetBlockGageRegenRate());
	UIHUDWidget->SetGold(PS->GetGold());

}

UTDSLHUDWidget* ATDSLPlayerController::GetHUD()
{
	return UIHUDWidget;
}

void ATDSLPlayerController::ShowEnemyInfoHUD(ATDSLCharacterBase* TargetCharacter)
{
	if (!UIEnemyInfoWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIEnemyInfoWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create once
	if (!UIEnemyInfoWidget)
	{
		UIEnemyInfoWidget = CreateWidget<UTDSLEnemyInfoWidget>(this, UIEnemyInfoWidgetClass);
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	if (!UIEnemyInfoWidget->IsInViewport()) 
	{
		UIEnemyInfoWidget->AddToViewport();
	}

	if (TimerHandle_HideEnemyInfoHUD.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_HideEnemyInfoHUD);
	}

	UIEnemyInfoWidget->SetCharacterName(TargetCharacter->GetCharacterName());
	float HealthPercent = TargetCharacter->GetHealth() / TargetCharacter->GetMaxHealth();
	UIEnemyInfoWidget->SetHealthPercentage(HealthPercent);

	if (HealthPercent <= 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HideEnemyInfoHUD, this, &ATDSLPlayerController::HideEnemyInfoHUD, 1.f, false, 1.f);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HideEnemyInfoHUD, this, &ATDSLPlayerController::HideEnemyInfoHUD, 1.f, false, 5.f);
	}
}

void ATDSLPlayerController::HideEnemyInfoHUD()
{
	if (UIEnemyInfoWidget->IsInViewport())
	{
		UIEnemyInfoWidget->RemoveFromViewport();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_HideEnemyInfoHUD);
	}
}

UTDSLEnemyInfoWidget* ATDSLPlayerController::GetEnemyHUD()
{
	return UIEnemyInfoWidget;
}

// Server only
void ATDSLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ATDSLPlayerState* PS = GetPlayerState<ATDSLPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void ATDSLPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Player is possessed.
	CreateHUD();
}
