// Copyright 2020 Dan Kestranek.


#include "GameplayAbility/AbilityTask/TDSLAT_WaitReceiveDamage.h"
#include "TDSLAbilitySystemComponent.h"

UTDSLAT_WaitReceiveDamage::UTDSLAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UTDSLAT_WaitReceiveDamage* UTDSLAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool InTriggerOnce)
{
	UTDSLAT_WaitReceiveDamage* MyObj = NewAbilityTask<UTDSLAT_WaitReceiveDamage>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UTDSLAT_WaitReceiveDamage::Activate()
{
	UTDSLAbilitySystemComponent* TDSLASC = Cast<UTDSLAbilitySystemComponent>(AbilitySystemComponent);

	if (TDSLASC)
	{
		TDSLASC->ReceivedDamage.AddDynamic(this, &UTDSLAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UTDSLAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	UTDSLAbilitySystemComponent* TDSLASC = Cast<UTDSLAbilitySystemComponent>(AbilitySystemComponent);

	if (TDSLASC)
	{
		TDSLASC->ReceivedDamage.RemoveDynamic(this, &UTDSLAT_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UTDSLAT_WaitReceiveDamage::OnDamageReceived(UTDSLAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}
