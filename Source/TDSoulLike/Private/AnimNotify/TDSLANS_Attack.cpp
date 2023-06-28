// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/TDSLANS_Attack.h"
#include "Characters/TDSLPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDSLANS_Attack)

UTDSLANS_Attack::UTDSLANS_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTDSLANS_Attack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SourceCharacter = Cast<ATDSLCharacterBase>(MeshComp->GetOwner());
	if (!SourceCharacter)
		return;
	// have to Move WeaponComponent on Playercharacter to CharacterBase later!!
	ATDSLPlayerCharacter* PC = Cast<ATDSLPlayerCharacter>(MeshComp->GetOwner());
	if (PC)
	{
		SourceWeaponMesh = PC->GetWeaponComponent();
	}

	PowerAtkTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Power"));
	AtkStackTag = FGameplayTag::RequestGameplayTag(FName("State.AtkStack"));
	DamageTag = FGameplayTag::RequestGameplayTag(FName("Damage.SetByCaller"));

	AtkStackCount = SourceCharacter->GetAbilitySystemComponent()->GetGameplayTagCount(AtkStackTag);
}

void UTDSLANS_Attack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!SourceCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SourceCharacter null"));
		return;
	}

	if (SourceCharacter->GetAbilitySystemComponent()->GetGameplayTagCount(PowerAtkTag) > 0)
	{
		switch (AtkStackCount)
		{
			case 0:
				ApplyDamageToTarget();
				break;
			case 1:
				ApplyDamageToTarget();
				break;
			case 2:
				ApplyDamageToTarget();
				break;
			case 3:
				ApplyDamageToTarget();
				break;
			case 4:
				ApplyDamageToTarget();
				break;
			case 5:
				ApplyDamageToTarget();
				break;
			case 6:
				ApplyDamageToTarget();
				break;
		}
	}
	else
	{
		switch (AtkStackCount)
		{
			case 1:
				ApplyDamageToTarget();
				break;
			case 2:
				ApplyDamageToTarget();
				break;
			case 3:
				ApplyDamageToTarget();
				break;
			case 4:
				ApplyDamageToTarget();
				break;
		}
	}
}

void UTDSLANS_Attack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

}

void UTDSLANS_Attack::ApplyDamageToTarget()
{
	FVector TraceStartPos = SourceWeaponMesh->GetSocketLocation(CollisionAttachSocket1);
	FVector TraceEndPos = SourceWeaponMesh->GetSocketLocation(CollisionAttachSocket2);

	TArray<FHitResult> OutResults;
	FCollisionShape MySphere = FCollisionShape::MakeSphere(CollisionSphereRadius);
	if (GetWorld()->SweepMultiByObjectType(OutResults, TraceStartPos, TraceEndPos, FQuat::Identity, ObjTypes.GetValue(), MySphere))
	{
		bool bIsExist = false;
		ATDSLCharacterBase* TargetCharacter;
		for (const FHitResult& out : OutResults)
		{
			TargetCharacter = Cast<ATDSLCharacterBase>(out.GetActor());
			if (TargetCharacter && AlreadyDamagedTargets.Contains(TargetCharacter))
			{
				bIsExist = true;
			}
		}
		if (!bIsExist)
		{
			AlreadyDamagedTargets.Add(TargetCharacter);
			// apply damage to target
			UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *TargetCharacter->GetName());

			UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
			if (TargetCharacter->IsAlive() && TargetASC)
			{
				FGameplayEffectSpecHandle GESpecHandle = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
					TargetASC->MakeOutgoingSpec(DamageEffect, 1, TargetASC->MakeEffectContext()),
					DamageTag,
					Damage * DamageCoefficient
				);
				TargetASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
			}
		}
	}
}
