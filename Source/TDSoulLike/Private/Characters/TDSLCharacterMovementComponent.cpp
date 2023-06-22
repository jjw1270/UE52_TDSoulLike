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

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	{
		return 0.0f;
	}

	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	if (RequestToStartBattlePosture)
	{
		return Owner->GetMoveSpeed() * BattlePostureSpeedMultiplier;
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

		MutableThis->ClientPredictionData = new FGDNetworkPredictionData_Client(*this);
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

void UTDSLCharacterMovementComponent::FGDSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartSprinting = false;
	SavedRequestToStartBattlePosture = false;
}

uint8 UTDSLCharacterMovementComponent::FGDSavedMove::GetCompressedFlags() const
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

bool UTDSLCharacterMovementComponent::FGDSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FGDSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartBattlePosture != ((FGDSavedMove*)&NewMove)->SavedRequestToStartBattlePosture)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UTDSLCharacterMovementComponent::FGDSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UTDSLCharacterMovementComponent* CharacterMovement = Cast<UTDSLCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
		SavedRequestToStartBattlePosture = CharacterMovement->RequestToStartBattlePosture;
	}
}

void UTDSLCharacterMovementComponent::FGDSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UTDSLCharacterMovementComponent* CharacterMovement = Cast<UTDSLCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UTDSLCharacterMovementComponent::FGDNetworkPredictionData_Client::FGDNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UTDSLCharacterMovementComponent::FGDNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FGDSavedMove());
}
