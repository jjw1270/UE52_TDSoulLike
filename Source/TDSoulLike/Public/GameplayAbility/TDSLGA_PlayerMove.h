// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/TDSLGameplayAbility.h"
#include "TDSLGA_PlayerMove.generated.h"

/**
 * This is an unfinished code. It will be implemented as a blueprint first and then written later.
 */
UCLASS()
class TDSOULLIKE_API UTDSLGA_PlayerMove : public UTDSLGameplayAbility
{
	GENERATED_BODY()
		
public:
	UTDSLGA_PlayerMove();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

private:
	UPROPERTY()
	class ACharacter* PlayerCharacter;

	UPROPERTY()
	class APlayerController* PlayerController;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> FXCursor;

	FVector CachedDestination;
	
	UPROPERTY()
	class UTDSLAT_OnTick* ATOnTick;

	UFUNCTION()
	void Move(float DeltaTime);

	FTimerHandle GetMovementStopHandle;
	FTimerDelegate OnMovementStopDelegate;

	void OnMovementStop(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);

};
