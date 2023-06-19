// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSLAbilitySystemComponent.h"


void UTDSLAbilitySystemComponent::ReceiveDamage(UTDSLAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
