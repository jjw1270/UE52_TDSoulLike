// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSDestructible.h"
#include "AbilitySystemComponent.h"
#include "GAS/TDSAttributeSet.h"


// Sets default values
ATDSDestructible::ATDSDestructible()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = StaticMeshComp;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UTDSAttributeSet>("AttributeSet");


}

// Called when the game starts or when spawned
void ATDSDestructible::BeginPlay()
{
	Super::BeginPlay();
	
	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ATDSDestructible::OnHealthAttributeChanged);
}

UAbilitySystemComponent* ATDSDestructible::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATDSDestructible::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.OldValue, Data.NewValue);
}

