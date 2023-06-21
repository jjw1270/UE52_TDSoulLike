// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "Abilities/Tasks/AbilityTask.h"
//#include "TDSLAT_WaitPlayerMoveToLocation.generated.h"
//
///**
// * 
// */
//UCLASS()
//class TDSOULLIKE_API UTDSLAT_WaitPlayerMoveToLocation : public UAbilityTask
//{
//	GENERATED_UCLASS_BODY()
//	
//public:
//	UTDSLAT_WaitPlayerMoveToLocation(const FObjectInitializer& ObjectInitializer);
//
//	UFUNCTION()
//	void OnPlayerReachedCallback();
//
//	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
//	static UTDSLAT_WaitPlayerMoveToLocation* AbilityTaskWaitPlayerMoveToLocation(
//			UGameplayAbility* OwningAbility,
//			FName TaskInstanceName
//		);
//
//	virtual void Activate() override;
//
//private:
//	virtual void OnDestroy(bool AbilityEnded) override;
//
//	UPROPERTY()
//	TObjectPtr<UCharacterMovementComponent> MovementComponent;
//
//};
