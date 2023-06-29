// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/TDSLGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

#include "Player/TDSLPlayerController.h"
#include "Characters/TDSLPlayerCharacter.h"
#include "Math/UnrealMathUtility.h"

UTDSLGameplayAbility::UTDSLGameplayAbility()
{	
	// Sets the ability to default to Instanced Per Actor.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Default tags that block this ability from activating
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

void UTDSLGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UTDSLGameplayAbility::StopActorMovement()
{
	if (AController* Controller = GetAvatarActorFromActorInfo()->GetInstigatorController())
	{
		Controller->StopMovement();
	}
}

FRotator UTDSLGameplayAbility::SetPlayerRotationToUnderCursor(bool SmoothRotation, float RotationSpeed)
{
	ATDSLPlayerCharacter* PlayerCharacter = Cast<ATDSLPlayerCharacter>(GetAvatarActorFromActorInfo());
	ATDSLPlayerController* PlayerController = Cast<ATDSLPlayerController>(PlayerCharacter->GetController());
	if (!PlayerCharacter || !PlayerController)
	{
		return FRotator::ZeroRotator;
	}

	FHitResult Hit;
	bool bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	FVector CachedDestination;

	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	FVector WorldDirection = (CachedDestination - PlayerCharacter->GetActorLocation()).GetSafeNormal();
	WorldDirection = FVector(WorldDirection.X, WorldDirection.Y, 0);
	
	FRotator WorldRotation = WorldDirection.Rotation();

	if (!SmoothRotation)
	{
		PlayerCharacter->SetActorRotation(WorldRotation);
	}
	else
	{
		FRotator TargetRotation;
		UWorld* World = GetWorld();
		if (World == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("GA World"));
			return FRotator::ZeroRotator;
		}

		//Set Timer to Smooth Rotation
		PlayerCharacter->GetWorldTimerManager().SetTimer(
			TimerHandle_StopRotation,
			//Loop Lamda Func
			[this, PlayerCharacter, &TargetRotation, WorldRotation, World, RotationSpeed]()
			{
				// Clear TimerHandle When Nearly Arrive TargetRotation
				if (FMath::IsNearlyEqual(
					PlayerCharacter->GetActorRotation().Yaw,
					WorldRotation.Yaw,
					2.f))
				{
					PlayerCharacter->GetWorldTimerManager().ClearTimer(TimerHandle_StopRotation);
					return;
				}
				//Smooth Rotation Using Interpolation
				TargetRotation = FMath::RInterpTo(
					PlayerCharacter->GetActorRotation(),
					WorldRotation,
					World->GetDeltaSeconds(),
					RotationSpeed
				);
				PlayerCharacter->SetActorRotation(TargetRotation);
			},
			World->GetDeltaSeconds(),
			true
		);
	}

	return WorldRotation;
}
