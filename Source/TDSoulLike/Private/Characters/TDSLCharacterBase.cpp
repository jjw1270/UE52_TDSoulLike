// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TDSLCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Characters/TDSLCharacterMovementComponent.h"
#include "UI/TDSLFloatingHpBarWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "TDSLAbilitySystemComponent.h"
#include "GameplayAbility/TDSLGameplayAbility.h"
#include "AttributeSets/TDSLAttributeSetBase.h"

#include "Player/TDSLPlayerController.h"


// Sets default values
ATDSLCharacterBase::ATDSLCharacterBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTDSLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	UIFloatingHPBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingHPBarComponent"));
	UIFloatingHPBarComponent->SetWidgetSpace(EWidgetSpace::World);
	UIFloatingHPBarComponent->SetupAttachment(RootComponent);
	UIFloatingHPBarComponent->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	UIFloatingHPBarComponent->SetRelativeRotation(FRotator(90.f, 0, 0));
	UIFloatingHPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);

	bAlwaysRelevant = true;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
}

UAbilitySystemComponent* ATDSLCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool ATDSLCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 ATDSLCharacterBase::GetAbilityLevel(ETDSLAbilityInputID AbilityID) const
{
	return 1;
}

void ATDSLCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

ETDSLHitReactDirection ATDSLCharacterBase::GetHitReactDirection(const FVector& ImpactPoint)
{
	const FVector& ActorLocation = GetActorLocation();
	// PointPlaneDist is super cheap - 1 vector subtraction, 1 dot product.
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorForwardVector());


	//if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	//{
		// Determine if Front or Back

		// Can see if it's left or right of Left/Right plane which would determine Front or Back
	//	if (DistanceToRightLeftPlane >= 0)
	//	{
	//		return ETDSLHitReactDirection::Front;
	//	}
	//	else
	//	{
	//		return ETDSLHitReactDirection::Back;
	//	}
	//}
	//else
	//{
	//	// Determine if Right or Left

	//	if (DistanceToFrontBackPlane >= 0)
	//	{
	//		return ETDSLHitReactDirection::Right;
	//	}
	//	else
	//	{
	//		return ETDSLHitReactDirection::Left;
	//	}
	//}

	return ETDSLHitReactDirection::Front;
}

void ATDSLCharacterBase::PlayHitReact_Implementation(FGameplayTag HitDirection, AActor* DamageCauser)
{
	if (IsAlive())
	{
		if (HitDirection == HitDirectionLeftTag)
		{
			ShowHitReact.Broadcast(ETDSLHitReactDirection::Left);
		}
		else if (HitDirection == HitDirectionFrontTag)
		{
			ShowHitReact.Broadcast(ETDSLHitReactDirection::Front);
		}
		else if (HitDirection == HitDirectionRightTag)
		{
			ShowHitReact.Broadcast(ETDSLHitReactDirection::Right);
		}
		else if (HitDirection == HitDirectionBackTag)
		{
			ShowHitReact.Broadcast(ETDSLHitReactDirection::Back);
		}
	}
}

bool ATDSLCharacterBase::PlayHitReact_Validate(FGameplayTag HitDirection, AActor* DamageCauser)
{
	return true;
}

float ATDSLCharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float ATDSLCharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ATDSLCharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float ATDSLCharacterBase::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

// Run on Server and all clients
void ATDSLCharacterBase::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->CancelAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void ATDSLCharacterBase::FinishDying()
{
	Destroy();
}

// Called when the game starts or when spawned
void ATDSLCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Setup FloatingHPBar UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	float HPBarDrawSize = GetCapsuleComponent()->GetScaledCapsuleRadius() * FloatingHPBarMultiSize;
	UIFloatingHPBarComponent->SetDrawSize(FVector2D(HPBarDrawSize));

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC != GetController())
	{
		if (UIFloatingHPBarClass)
		{
			UIFloatingHPBar = CreateWidget<UTDSLFloatingHpBarWidget>(PC, UIFloatingHPBarClass);
			if (UIFloatingHPBar && UIFloatingHPBarComponent)
			{
				UIFloatingHPBarComponent->SetWidget(UIFloatingHPBar);
				UE_LOG(LogTemp, Warning, TEXT("adf"));

				// Setup the floating HP bar
				UIFloatingHPBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
			}
		}
	}

	// Attribute change callbacks
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ATDSLCharacterBase::HealthChanged);

}

void ATDSLCharacterBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	if (UIFloatingHPBar)
	{
		UIFloatingHPBar->SetHealthPercentage(Health / GetMaxHealth());
	}

	// If the enemy died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		Die();
	}
}

void ATDSLCharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UTDSLGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ATDSLCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ATDSLCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !GetAbilitySystemComponent() || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void ATDSLCharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}
