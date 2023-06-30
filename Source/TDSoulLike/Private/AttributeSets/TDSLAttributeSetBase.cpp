// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/TDSLAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Characters/TDSLCharacterBase.h"
#include "Characters/TDSLPlayerCharacter.h"

UTDSLAttributeSetBase::UTDSLAttributeSetBase()
{
	// Cache tags
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
}

void UTDSLAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetMaxBlockGageAttribute())
	{
		AdjustAttributeForMaxChange(BlockGage, MaxBlockGage, NewValue, GetBlockGageAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		NewValue = FMath::Clamp<float>(NewValue, 100, 1000);
	}
}

void UTDSLAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ATDSLCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ATDSLCharacterBase>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	ATDSLCharacterBase* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<ATDSLCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<ATDSLCharacterBase>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	// TODO -- Add some logic after Gameplay Effect executed

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.0f)
		{
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
			}

			const FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag(FName("State.Block"), false);
			//bool IsBlocking = Data.EffectSpec.CapturedSourceTags.GetSpecTags().HasTagExact(BlockTag);

			if (SourceTags.HasTag(BlockTag))
			{
				const FGameplayTag ParryingTag = FGameplayTag::RequestGameplayTag(FName("State.Block.Parrying"), false);
				if (SourceTags.HasTag(ParryingTag))
				{
					// Parrying doesnt take any damage
					UE_LOG(LogTemp, Warning, TEXT("Parrying"));
				}
				else
				{
					// need fix when block gage is zero
					UE_LOG(LogTemp, Warning, TEXT("Blocking"));

					// Apply the BlokGage/Health change and then clamp it
					const float NewBlockGage = GetBlockGage() - LocalDamageDone * 0.7;
					SetBlockGage(FMath::Clamp(NewBlockGage, 0.0f, GetMaxBlockGage()));
					const float NewHealth = GetHealth() - LocalDamageDone * 0.3;
					SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
				}
			}
			else
			{
				// Apply the health change and then clamp it
				const float NewHealth = GetHealth() - LocalDamageDone;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			}


			if (TargetCharacter && WasAlive)
			{
				// This is the log statement for damage received. Turned off for live games.
				// UE_LOG(LogTemp, Log, TEXT("%s Damage Received: %f"), *TargetCharacter->GetName(), LocalDamageDone);

				// Play HitReact animation and sound with a multicast RPC.
				const FHitResult* Hit = Data.EffectSpec.GetContext().GetHitResult();

				if (Hit)
				{
					ETDSLHitReactDirection HitDirection = TargetCharacter->GetHitReactDirection(Hit->Location);

					switch (HitDirection)
					{
						case ETDSLHitReactDirection::Front:
							TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
							break;
						case ETDSLHitReactDirection::Back:
							TargetCharacter->PlayHitReact(HitDirectionBackTag, SourceCharacter);
							break;
					}
				}
				else
				{
					// No hit result. Default to front.
					TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
				}

				/* Show damage number for the Source player unless it was self damage
				if (SourceActor != TargetActor)
				{
					ATDSLPlayerController* PC = Cast<ATDSLPlayerController>(SourceController);
					if (PC)
					{
						PC->ShowDamageNumber(LocalDamageDone, TargetCharacter);
					}
				}*/

				//if (!TargetCharacter->IsAlive())
				//{
				//	// TargetCharacter was alive before this damage and now is not alive, give Gold bounties to Source.
				//	// Don't give bounty to self.
				//	if (SourceController != TargetController)
				//	{
				//		// Create a dynamic instant Gameplay Effect to give the bounties
				//		UGameplayEffect* GEBounty = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Bounty")));
				//		GEBounty->DurationPolicy = EGameplayEffectDurationType::Instant;

				//		int32 Idx = GEBounty->Modifiers.Num();
				//		GEBounty->Modifiers.SetNum(Idx + 2);

				//		FGameplayModifierInfo& InfoGold = GEBounty->Modifiers[Idx + 1];
				//		InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
				//		InfoGold.ModifierOp = EGameplayModOp::Additive;
				//		InfoGold.Attribute = UTDSLAttributeSetBase::GetGoldAttribute();

				//		Source->ApplyGameplayEffectToSelf(GEBounty, 1.0f, Source->MakeEffectContext());
				//	}
				//}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes.
		// Health loss should go through Damage.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Handle stamina changes.
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetBlockGageAttribute())
	{
		// Handle BlockGage changes.
		SetBlockGage(FMath::Clamp(GetBlockGage(), 0.0f, GetMaxBlockGage()));
	}
}

void UTDSLAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, BlockGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, MaxBlockGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, BlockGageRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTDSLAttributeSetBase, Gold, COND_None, REPNOTIFY_Always);

}

void UTDSLAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();

	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilitySystemComponent)
	{
		// Change current value to maintain the Current Value / Maximum Value percentage.
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UTDSLAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, Health, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MaxHealth, OldValue);
}

void UTDSLAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, HealthRegenRate, OldValue);
}

void UTDSLAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, Stamina, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MaxStamina, OldValue);
}

void UTDSLAttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, StaminaRegenRate, OldValue);
}

void UTDSLAttributeSetBase::OnRep_BlockGage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, BlockGage, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MaxBlockGage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MaxBlockGage, OldValue);
}

void UTDSLAttributeSetBase::OnRep_BlockGageRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, BlockGageRegenRate, OldValue);
}

void UTDSLAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, MoveSpeed, OldMoveSpeed);
}

void UTDSLAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTDSLAttributeSetBase, Gold, OldValue);
}
