// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TDSoulLike/TDSoulLike.h"
#include "TDSLGameplayAbility.generated.h"


UCLASS()
class TDSOULLIKE_API UTDSLGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UTDSLGameplayAbility();

	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	ETDSLAbilityInputID AbilityInputID = ETDSLAbilityInputID::None;

	// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	ETDSLAbilityInputID AbilityID = ETDSLAbilityInputID::None;

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable)
	void StopActorMovement();

	UFUNCTION(BlueprintCallable)
	FRotator SetPlayerRotationToUnderCursor(bool SmoothRotation, float RotationSpeed = 0);

private:
	FTimerHandle TimerHandle_StopRotation;
};
