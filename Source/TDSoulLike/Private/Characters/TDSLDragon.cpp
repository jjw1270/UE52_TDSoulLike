// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TDSLDragon.h"
#include "TDSLAbilitySystemComponent.h"
#include "AttributeSets/TDSLAttributeSetBase.h"

//#include "Kismet/GameplayStatics.h"

ATDSLDragon::ATDSLDragon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Create ability system component, and set it to be explicitly replicated
	HardRefAbilitySystemComponent = CreateDefaultSubobject<UTDSLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HardRefAbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	HardRefAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Set our parent's TWeakObjectPtr
	AbilitySystemComponent = HardRefAbilitySystemComponent;

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	HardRefAttributeSetBase = CreateDefaultSubobject<UTDSLAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set our parent's TWeakObjectPtr
	AttributeSetBase = HardRefAttributeSetBase;

}

void ATDSLDragon::BeginPlay()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();
	}

	Super::BeginPlay();
}

void ATDSLDragon::Die()
{
	Super::Die();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
