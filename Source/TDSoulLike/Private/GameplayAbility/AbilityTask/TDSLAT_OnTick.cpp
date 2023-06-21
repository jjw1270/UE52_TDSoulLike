// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AbilityTask/TDSLAT_OnTick.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(TDSLAT_OnTick)

UTDSLAT_OnTick::UTDSLAT_OnTick(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = 1;
}

UTDSLAT_OnTick* UTDSLAT_OnTick::AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UTDSLAT_OnTick* MyObj = NewAbilityTask<UTDSLAT_OnTick>(OwningAbility);
	return MyObj;
}

void UTDSLAT_OnTick::Activate()
{
	Super::Activate();
}

void UTDSLAT_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	OnTick.Broadcast(DeltaTime);
}
