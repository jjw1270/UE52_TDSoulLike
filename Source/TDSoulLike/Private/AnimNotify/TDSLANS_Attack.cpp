// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/TDSLANS_Attack.h"
#include "Characters/TDSLPlayerCharacter.h"
#include "GameplayAbility/TDSLGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TDSLPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDSLANS_Attack)

UTDSLANS_Attack::UTDSLANS_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif // WITH_EDITORONLY_DATA

	PowerAtkTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Power"));
	AtkStackTag = FGameplayTag::RequestGameplayTag(FName("State.AtkStack"));
	DamageTag = FGameplayTag::RequestGameplayTag(FName("Damage.SetByCaller"));
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
	AlreadyDamagedTargets.Empty();
}

void UTDSLANS_Attack::ApplyDamageToTarget()
{
	FVector TraceStartPos;
	FVector TraceEndPos;
	if (bIsSword)
	{
		TraceStartPos = SourceWeaponMesh->GetSocketLocation(CollisionAttachSocket1);
		TraceEndPos = SourceWeaponMesh->GetSocketLocation(CollisionAttachSocket2);
	}
	else
	{
		TraceStartPos = SourceCharacter->GetMesh()->GetSocketLocation(CollisionAttachSocket1);
		TraceEndPos = SourceCharacter->GetMesh()->GetSocketLocation(CollisionAttachSocket2);
	}

	TArray<FHitResult> OutResults;
	FCollisionShape MySphere = FCollisionShape::MakeSphere(CollisionSphereRadius);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(SourceCharacter);
	bool IsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(SourceCharacter->GetWorld(), TraceStartPos, TraceEndPos, CollisionSphereRadius,
		ObjTypes, false, ActorsToIgnore, DrawDebugCollision.GetValue(), OutResults, true, FColor::Red, FColor::Green, 1.f);

	if (IsHit)
	{
		bool bIsExist = false;
		FHitResult OutResult;
		ATDSLCharacterBase* TargetCharacter;
		for (const FHitResult& out : OutResults)
		{
			TargetCharacter = Cast<ATDSLCharacterBase>(out.GetActor());
			OutResult = out;
			if (TargetCharacter && AlreadyDamagedTargets.Contains(TargetCharacter))
			{
				bIsExist = true;
			}
		}
		if (!bIsExist)
		{
			AlreadyDamagedTargets.Add(TargetCharacter);
			// apply damage to target
			UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
			if (TargetCharacter->IsAlive() && TargetASC)
			{
				FGameplayEffectSpecHandle DamageEffectSpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, 1, TargetASC->MakeEffectContext());

				// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
				DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, Damage * DamageCoefficient);

				FHitResult SourceInfo;
				SourceInfo.Location = SourceCharacter->GetActorLocation();
				DamageEffectSpecHandle.Data.Get()->GetContext().AddHitResult(SourceInfo);

				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());

				ATDSLPlayerController* PCController = Cast<ATDSLPlayerController>(SourceCharacter->GetController());
				if (PCController)
				{
					PCController->ShowEnemyInfoHUD(TargetCharacter);
				}
			}
		}
	}
}
