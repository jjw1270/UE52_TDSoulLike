// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/TDSLGA_ShowTargetInfo.h"
#include "Characters/TDSLPlayerCharacter.h"
#include "Player/TDSLPlayerController.h"
#include "UI/TDSLEnemyInfoWidget.h"

UTDSLGA_ShowTargetInfo::UTDSLGA_ShowTargetInfo()
{
	AbilityInputID = ETDSLAbilityInputID::Move;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.ShowTargetInfo")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.ShowTargetInfo")));
}

void UTDSLGA_ShowTargetInfo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		FHitResult Hit;
		bool bHitSuccessful = GetActorInfo().PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

		if (bHitSuccessful && Hit.GetActor() != GetAvatarActorFromActorInfo())
		{
			ATDSLCharacterBase* TargetActor = Cast<ATDSLCharacterBase>(Hit.GetActor());
			ATDSLPlayerController* PC = Cast<ATDSLPlayerController>(GetActorInfo().PlayerController);
			if (TargetActor && PC)
			{
				PC->ShowEnemyInfoHUD(TargetActor);
			}
		}

		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

bool UTDSLGA_ShowTargetInfo::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UTDSLGA_ShowTargetInfo::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void UTDSLGA_ShowTargetInfo::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UTDSLGA_ShowTargetInfo::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
