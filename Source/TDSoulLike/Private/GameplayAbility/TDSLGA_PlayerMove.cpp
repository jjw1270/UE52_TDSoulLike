// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/TDSLGA_PlayerMove.h"
#include "GameplayAbility/AbilityTask/TDSLAT_OnTick.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "TDSoulLike/TDSoulLike.h"

#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

UTDSLGA_PlayerMove::UTDSLGA_PlayerMove()
{
	AbilityInputID = ETDSLAbilityInputID::Move;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.PlayerMove")));
}

void UTDSLGA_PlayerMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		PlayerCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		PlayerController = CastChecked<APlayerController>(ActorInfo->PlayerController.Get());

		if (!PlayerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("pc nullptr"));
			return;
		}

		if (!AT_OnTick)
		{
			UE_LOG(LogTemp, Warning, TEXT("AT_OnTick nullptr"));
			return;
		}

		//ATWaitInputRelease = NewObject<UAbilityTask_WaitInputRelease>(this);
		//ATWaitInputRelease->OnRelease.AddDynamic(this, &UTDSLGA_PlayerMove::Move);
		//AT_OnTick = NewObject<UTDSLAT_OnTick>(this);

		//AT_OnTick->OnTick.BindUFunction(this, FName("Move"));
		//AT_OnTick->Activate();

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
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
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



	UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
	// We move there and spawn some particles
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	//// If it was a short press
	//if (FollowTime <= ShortPressThreshold)
	//{ }
	//FollowTime = 0.f;
}

void UTDSLGA_PlayerMove::Move()
{
	// We flag that the input is being pressed
	//FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer
	FVector WorldDirection = (CachedDestination - PlayerCharacter->GetActorLocation()).GetSafeNormal();

	// Move to Target
	PlayerCharacter->AddMovementInput(WorldDirection, 1.0, false);
}
