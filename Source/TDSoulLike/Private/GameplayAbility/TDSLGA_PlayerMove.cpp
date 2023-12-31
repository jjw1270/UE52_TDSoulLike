// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/TDSLGA_PlayerMove.h"
#include "GameplayAbility/AbilityTask/TDSLAT_OnTick.h"
#include "GameplayAbility/AbilityTask/TDSLAT_WaitPlayerStop.h"

#include "TDSoulLike/TDSoulLike.h"
#include "TDSLAbilitySystemComponent.h"

#include "Characters/TDSLCharacterBase.h"
#include "Player/TDSLPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

UTDSLGA_PlayerMove::UTDSLGA_PlayerMove()
{
	AbilityInputID = ETDSLAbilityInputID::Move;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.PlayerMove")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.PlayerMove")));
}

void UTDSLGA_PlayerMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		PlayerCharacter = CastChecked<ATDSLCharacterBase>(ActorInfo->AvatarActor.Get());
		PlayerController = CastChecked<ATDSLPlayerController>(ActorInfo->PlayerController);

		ATOnTick = UTDSLAT_OnTick::AbilityTaskOnTick(this, FName());
		ATOnTick->OnTick.AddDynamic(this, &UTDSLGA_PlayerMove::Move);
		ATOnTick->ReadyForActivation();

		ATWaitPlayerStop = UTDSLAT_WaitPlayerStop::CreateWaitVelocityChange(this, FName());
		ATWaitPlayerStop->OnVelocityChage.AddDynamic(this, &UTDSLGA_PlayerMove::OnMovementStop);

	}
}

bool UTDSLGA_PlayerMove::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	return true;
}

void UTDSLGA_PlayerMove::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		ATOnTick->EndTask();
		if (TargetActor)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToActor(PlayerController, TargetActor);
		}
		else
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}

		ATWaitPlayerStop->ReadyForActivation();
	}
}

void UTDSLGA_PlayerMove::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UTDSLGA_PlayerMove::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UTDSLGA_PlayerMove::Move(float DeltaTime)
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		if (Hit.GetActor() != PlayerCharacter)
		{
			TargetActor = Cast<ATDSLCharacterBase>(Hit.GetActor());
		}
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer
	FVector WorldDirection = (CachedDestination - PlayerCharacter->GetActorLocation()).GetSafeNormal();

	// Move to Target
	PlayerCharacter->AddMovementInput(WorldDirection, 1.0, false);

	if (TargetActor)
	{
		PlayerController->ShowEnemyInfoHUD(TargetActor);
	}
}

void UTDSLGA_PlayerMove::OnMovementStop()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
