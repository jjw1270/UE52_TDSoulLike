// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TDSLPlayerCharacter.h"
#include "Player/TDSLPlayerController.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

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

	//UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	//UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	//UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	//UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));
	
	//UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASDocumentation/UI/UI_FloatingStatusBar_Hero.UI_FloatingStatusBar_Hero_C"));
	//if (!UIFloatingStatusBarClass)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	//}

	//AIControllerClass = AGDHeroAIController::StaticClass();

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void ATDSLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

		//AGDPlayerController* PC = Cast<AGDPlayerController>(GetController());
		//if (PC)
		//{
		//	PC->CreateHUD();
		//}

		//InitializeFloatingStatusBar();
	}

}

USkeletalMeshComponent* ATDSLPlayerCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}

float ATDSLPlayerCharacter::GetBlockGage() const
{
	return 0.0f;
}

float ATDSLPlayerCharacter::GetMaxBlockGage() const
{
	return 0.0f;
}

float ATDSLPlayerCharacter::GetBlockGageRegen() const
{
	return 0.0f;
}

float ATDSLPlayerCharacter::GetGold() const
{
	return 0.0f;
}

void ATDSLPlayerCharacter::SetBlockGage(float BlockGage)
{
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
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}



	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	// InitializeFloatingStatusBar();
}

void ATDSLPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent && GetMesh())
	{
		WeaponComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponBackSocket"));
	}
}

void ATDSLPlayerCharacter::InitializeFloatingStatusBar()
{
	//// Only create once
	//if (UIFloatingStatusBar || !AbilitySystemComponent.IsValid())
	//{
	//	return;
	//}

	//// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	//AGDPlayerController* PC = Cast<AGDPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (PC && PC->IsLocalPlayerController())
	//{
	//	if (UIFloatingStatusBarClass)
	//	{
	//		UIFloatingStatusBar = CreateWidget<UGDFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
	//		if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
	//		{
	//			UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

	//			// Setup the floating status bar
	//			UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
	//			UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
	//		}
	//	}
	//}
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

		/*AGDPlayerController* PC = Cast<AGDPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}*/

		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		InitializeFloatingStatusBar();


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
