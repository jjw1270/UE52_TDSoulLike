// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TDSLAT_WaitOverlap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitOverlapDelegate, const FGameplayAbilityTargetDataHandle&, TargetData);

class AActor;
class UPrimitiveComponent;

/**
 *	Fixme: this is still incomplete and probablyh not what most games want for melee systems.
 *		-Only actually activates on Blocking hits
 *		-Uses first PrimitiveComponent instead of being able to specify arbitrary component.
 */
UCLASS()
class TDSOULLIKE_API UTDSLAT_WaitOverlap : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitOverlapDelegate	OnOverlap;

	UPrimitiveComponent* CollisionComponent;

	bool bIsRepeat;

	UFUNCTION()
	void OnOverlapCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Activate() override;

	/** Wait until an overlap occurs. This will need to be better fleshed out so we can specify game specific collision requirements */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UTDSLAT_WaitOverlap* WaitForOverlap(UGameplayAbility* OwningAbility, UPrimitiveComponent* CollisionComponent, bool IsRepeat);

private:

	virtual void OnDestroy(bool AbilityEnded) override;

};
