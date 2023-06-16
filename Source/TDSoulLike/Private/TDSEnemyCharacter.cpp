// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GAS/TDSGameplayAbility.h"
#include "GAS/TDSAttributeSet.h"

// Sets default values
ATDSEnemyCharacter::ATDSEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UTDSAttributeSet>("AttributeSet");

}

// Called when the game starts or when spawned
void ATDSEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ATDSEnemyCharacter::OnHealthAttributeChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetArmorAttribute()).AddUObject(this, &ATDSEnemyCharacter::OnArmorAttributeChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetChilledAttribute()).AddUObject(this, &ATDSEnemyCharacter::OnChilledAttributeChanged);

	AttributeSet->OnOutOfHealth.AddUObject(this, &ATDSEnemyCharacter::OnOutOfHealthChanged);
	AttributeSet->OnOutOfArmor.AddUObject(this, &ATDSEnemyCharacter::OnOutOfArmorChanged);
}

UAbilitySystemComponent* ATDSEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATDSEnemyCharacter::InitializeAbilities()
{
	// Give Abilities, Server only
	if (!HasAuthority() || !AbilitySystemComponent)
		return;

	for (TSubclassOf<UTDSGameplayAbility>& Ability : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this));
	}
}

void ATDSEnemyCharacter::InitializeEffects()
{
	if (!AbilitySystemComponent)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
	{
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ATDSEnemyCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.OldValue, Data.NewValue);
}

void ATDSEnemyCharacter::OnArmorAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnArmorChanged(Data.OldValue, Data.NewValue);
}

void ATDSEnemyCharacter::OnOutOfHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnOutOfHealth(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);
}

void ATDSEnemyCharacter::OnOutOfArmorChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnOutOfArmor(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);
}

void ATDSEnemyCharacter::OnChilledAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnChilledChanged(Data.OldValue, Data.NewValue);
}

void ATDSEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeEffects();
	InitializeAbilities();
}

// Called every frame
void ATDSEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATDSEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

