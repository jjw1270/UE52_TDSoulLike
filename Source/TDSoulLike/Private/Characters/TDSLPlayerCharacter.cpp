// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TDSLPlayerCharacter.h"
#include "Player/TDSLPlayerController.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "AI/TDSLPlayerAIController.h"
#include "Player/TDSLPlayerState.h"
#include "TDSLAbilitySystemComponent.h"
#include "AttributeSets/TDSLAttributeSetBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

ATDSLPlayerCharacter::ATDSLPlayerCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	// CameraBoom->SetRelativeLocation(FVector(0, 0, 68.492264));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	// FollowCamera->FieldOfView = 80.0f;

	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Weapon"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	AIControllerClass = ATDSLPlayerAIController::StaticClass();

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void ATDSLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATDSLPlayerCharacter::OnSetDestinationStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATDSLPlayerCharacter::OnSetDestinationAbility);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ATDSLPlayerCharacter::OnDashAbility);

	}

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();
}

void ATDSLPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ATDSLPlayerState* PS = GetPlayerState<ATDSLPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UTDSLAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/BlockGage to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetBlockGage(GetBlockGage());

		// End respawn specific things


		AddStartupEffects();

		AddCharacterAbilities();

		ATDSLPlayerController* PC = Cast<ATDSLPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}
	}
}

float ATDSLPlayerCharacter::GetBlockGage() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetBlockGage();
	}

	return 0.0f;
}

float ATDSLPlayerCharacter::GetMaxBlockGage() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxBlockGage();
	}

	return 0.0f;
}

void ATDSLPlayerCharacter::SetBlockGage(float BlockGage)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetBlockGage(BlockGage);
	}
}

void ATDSLPlayerCharacter::FinishDying()
{
	//if (GetLocalRole() == ROLE_Authority)
	//{
	//	AGASDocumentationGameMode* GM = Cast<AGASDocumentationGameMode>(GetWorld()->GetAuthGameMode());

	//	if (GM)
	//	{
	//		GM->HeroDied(GetController());
	//	}
	//}

	//Super::FinishDying();
}

/**
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void ATDSLPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATDSLPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponBackSocketName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponBackSocketName Empty"));
		return;
	}

	if (WeaponComponent && GetMesh())
	{
		WeaponComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBackSocketName);
	}
}

// Client only
void ATDSLPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ATDSLPlayerState* PS = GetPlayerState<ATDSLPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UTDSLAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		BindASCInput();

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		ATDSLPlayerController* PC = Cast<ATDSLPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetBlockGage(GetMaxBlockGage());
	}
}

void ATDSLPlayerCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/TDSoulLike"), FName("ETDSLAbilityInputID"));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), AbilityEnumAssetPath, static_cast<int32>(ETDSLAbilityInputID::Confirm), static_cast<int32>(ETDSLAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}

void ATDSLPlayerCharacter::OnSetDestinationStarted()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->StopMovement();
	}
}

void ATDSLPlayerCharacter::OnSetDestinationAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(ETDSLAbilityInputID::Move));
}

void ATDSLPlayerCharacter::OnDashAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(ETDSLAbilityInputID::Dash));
}

void ATDSLPlayerCharacter::SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID)
{
	if (AbilitySystemComponent.IsValid())
		return;

	if (Value.Get<bool>())
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}
