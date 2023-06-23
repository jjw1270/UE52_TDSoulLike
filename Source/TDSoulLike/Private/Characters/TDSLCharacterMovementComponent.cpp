// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TDSLCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Characters/TDSLCharacterBase.h"
#include "GameplayTagContainer.h"

UTDSLCharacterMovementComponent::UTDSLCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
	BattlePostureSpeedMultiplier = 0.5f;
}

float UTDSLCharacterMovementComponent::GetMaxSpeed() const
{
	ATDSLCharacterBase* Owner = Cast<ATDSLCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Block"))))
	{
		return 0.0f;
	}

	if (RequestToStartBattlePosture)
	{
		if (RequestToStartSprinting)
		{
			return Owner->GetMoveSpeed() * BattlePostureSpeedMultiplier * SprintSpeedMultiplier;
		}
		else
		{
			return Owner->GetMoveSpeed() * BattlePostureSpeedMultiplier;
		}
	}

	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
}

void UTDSLCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartBattlePosture = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UTDSLCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UTDSLCharacterMovementComponent* MutableThis = const_cast<UTDSLCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FTDSLNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UTDSLCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UTDSLCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

void UTDSLCharacterMovementComponent::StartBattlePosture()
{
	RequestToStartBattlePosture = true;
}

void UTDSLCharacterMovementComponent::StopBattlePosture()
{
	RequestToStartBattlePosture = false;
}

void UTDSLCharacterMovementComponent::FTDSLSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartSprinting = false;
	SavedRequestToStartBattlePosture = false;
}

uint8 UTDSLCharacterMovementComponent::FTDSLSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartBattlePosture)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool UTDSLCharacterMovementComponent::FTDSLSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FTDSLSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartBattlePosture != ((FTDSLSavedMove*)&NewMove)->SavedRequestToStartBattlePosture)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UTDSLCharacterMovementComponent::FTDSLSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UTDSLCharacterMovementComponent* CharacterMovement = Cast<UTDSLCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
		SavedRequestToStartBattlePosture = CharacterMovement->RequestToStartBattlePosture;
	}
}

void UTDSLCharacterMovementComponent::FTDSLSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UTDSLCharacterMovementComponent* CharacterMovement = Cast<UTDSLCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UTDSLCharacterMovementComponent::FTDSLNetworkPredictionData_Client::FTDSLNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UTDSLCharacterMovementComponent::FTDSLNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FTDSLSavedMove());
}
