// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AbilityTask/TDSLAT_WaitOverlap.h"
#include "Components/PrimitiveComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDSLAT_WaitOverlap)

UTDSLAT_WaitOverlap::UTDSLAT_WaitOverlap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTDSLAT_WaitOverlap::OnOverlapCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// Construct TargetData
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);

		// Give it a handle and return
		FGameplayAbilityTargetDataHandle	Handle;
		Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnOverlap.Broadcast(Handle);
		}
		
		if (!bIsRepeat)
		{
			EndTask();
		}
	}
}

UTDSLAT_WaitOverlap* UTDSLAT_WaitOverlap::WaitForOverlap(UGameplayAbility* OwningAbility, UPrimitiveComponent* CollisionComponent, bool IsRepeat)
{
	UTDSLAT_WaitOverlap* MyObj = NewAbilityTask<UTDSLAT_WaitOverlap>(OwningAbility);
	MyObj->CollisionComponent = CollisionComponent;
	MyObj->bIsRepeat = IsRepeat;
	return MyObj;
}

void UTDSLAT_WaitOverlap::Activate()
{
	SetWaitingOnAvatar();

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UTDSLAT_WaitOverlap::OnOverlapCallback);
	}
}

void UTDSLAT_WaitOverlap::OnDestroy(bool AbilityEnded)
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UTDSLAT_WaitOverlap::OnOverlapCallback);
	}

	Super::OnDestroy(AbilityEnded);
}
