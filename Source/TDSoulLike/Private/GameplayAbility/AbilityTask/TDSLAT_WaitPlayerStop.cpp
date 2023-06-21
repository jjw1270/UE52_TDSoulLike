// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AbilityTask/TDSLAT_WaitPlayerStop.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDSLAT_WaitPlayerStop)

UTDSLAT_WaitPlayerStop::UTDSLAT_WaitPlayerStop(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}


void UTDSLAT_WaitPlayerStop::TickTask(float DeltaTime)
{
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATWaitPlayerStop Chracter"));
		EndTask();
	}

	if (Character->GetVelocity().Equals(FVector::ZeroVector))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnVelocityChage.Broadcast();
		}
		EndTask();
	}
}

UTDSLAT_WaitPlayerStop* UTDSLAT_WaitPlayerStop::CreateWaitVelocityChange(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UTDSLAT_WaitPlayerStop* MyObj = NewAbilityTask<UTDSLAT_WaitPlayerStop>(OwningAbility);
	return MyObj;
}

void UTDSLAT_WaitPlayerStop::Activate()
{
	Character = Cast<ACharacter>(GetAvatarActor());
	SetWaitingOnAvatar();
}
