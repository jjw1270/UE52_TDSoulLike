// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TDSLPlayerState.h"
#include "TDSLAbilitySystemComponent.h"
#include "AttributeSets/TDSLAttributeSetBase.h"

ATDSLPlayerState::ATDSLPlayerState()
{	
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UTDSLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UTDSLAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 60.0f;

	//cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

}

UAbilitySystemComponent* ATDSLPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTDSLAttributeSetBase* ATDSLPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool ATDSLPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void ATDSLPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	// TODO -- implement HUD later
}

float ATDSLPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float ATDSLPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float ATDSLPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float ATDSLPlayerState::GetBlockGage() const
{
	return AttributeSetBase->GetBlockGage();
}

float ATDSLPlayerState::GetMaxBlockGage() const
{
	return AttributeSetBase->GetMaxBlockGage();
}

float ATDSLPlayerState::GetBlockGageRegenRate() const
{
	return AttributeSetBase->GetBlockGageRegenRate();
}

float ATDSLPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

int32 ATDSLPlayerState::GetGold() const
{
	return AttributeSetBase->GetGold();
}

void ATDSLPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ATDSLPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &ATDSLPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &ATDSLPlayerState::HealthRegenRateChanged);
		BlockGageChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetBlockGageAttribute()).AddUObject(this, &ATDSLPlayerState::BlockGageChanged);
		MaxBlockGageChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxBlockGageAttribute()).AddUObject(this, &ATDSLPlayerState::MaxBlockGageChanged);
		BlockGageRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetBlockGageRegenRateAttribute()).AddUObject(this, &ATDSLPlayerState::BlockGageRegenRateChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).AddUObject(this, &ATDSLPlayerState::GoldChanged);
	}
}

void ATDSLPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
}

void ATDSLPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
}

void ATDSLPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData& Data)
{
}

void ATDSLPlayerState::BlockGageChanged(const FOnAttributeChangeData& Data)
{
}

void ATDSLPlayerState::MaxBlockGageChanged(const FOnAttributeChangeData& Data)
{
}

void ATDSLPlayerState::BlockGageRegenRateChanged(const FOnAttributeChangeData& Data)
{
}

void ATDSLPlayerState::GoldChanged(const FOnAttributeChangeData& Data)
{
}
