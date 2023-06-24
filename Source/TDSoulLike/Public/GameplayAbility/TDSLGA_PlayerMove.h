// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/TDSLGameplayAbility.h"
#include "TDSLGA_PlayerMove.generated.h"

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
	//FGameplayTag BlockTag;

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

	UPROPERTY()
	class UTDSLAT_WaitPlayerStop* ATWaitPlayerStop;

	//UPROPERTY()
	//class UAbilityTask_WaitGameplayTagAdded* ATWaitTag;

	UFUNCTION()
	void OnMovementStop();
};
