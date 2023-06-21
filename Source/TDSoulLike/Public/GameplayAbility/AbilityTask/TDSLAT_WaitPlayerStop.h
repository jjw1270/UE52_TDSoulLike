// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TDSLAT_WaitPlayerStop.generated.h"


class UMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitPlayerStopDelegate);

UCLASS()
class TDSOULLIKE_API UTDSLAT_WaitPlayerStop : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Delegate called when velocity requirements are met */
	UPROPERTY(BlueprintAssignable)
	FWaitPlayerStopDelegate OnVelocityChage;

	virtual void TickTask(float DeltaTime) override;

	/** Wait for the actor's movement component velocity to be of minimum magnitude when projected along given direction */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitVelocityChange", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UTDSLAT_WaitPlayerStop* CreateWaitVelocityChange(UGameplayAbility* OwningAbility, FName TaskInstanceName);

	virtual void Activate() override;

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> Character;
};
