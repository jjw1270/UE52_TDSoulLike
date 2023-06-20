// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/TDSLCharacterBase.h"
#include "InputActionValue.h"
#include "TDSLPlayerCharacter.generated.h"


class UInputAction;

/**
 * A player or AI controlled hero character.
 */
UCLASS()
class TDSOULLIKE_API ATDSLPlayerCharacter : public ATDSLCharacterBase
{
	GENERATED_BODY()
	
public:
	ATDSLPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE USkeletalMeshComponent* GetWeaponComponent() const { return WeaponComponent; }

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponBackSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponRightHandSocketName;

	virtual void FinishDying() override;

	UFUNCTION(BlueprintCallable, Category = "GAS|Chararcter|Attributes")
	float GetBlockGage() const;

	UFUNCTION(BlueprintCallable, Category = "GAS|Chararcter|Attributes")
	float GetMaxBlockGage() const;

protected:
	virtual void SetBlockGage(float BlockGage);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USkeletalMeshComponent* WeaponComponent;

	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// Client only
	virtual void OnRep_PlayerState() override;

	// Called from both SetupPlayerInputComponent and OnRep_PlayerState because of a potential race condition where the PlayerController might
	// call ClientRestart which calls SetupPlayerInputComponent before the PlayerState is repped to the client so the PlayerState would be null in SetupPlayerInputComponent.
	// Conversely, the PlayerState might be repped before the PlayerController calls ClientRestart so the Actor's InputComponent would be null in OnRep_PlayerState.
	void BindASCInput();

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	/** Right Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SetDestinationClickAction;

	/** Block Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> BlockAction;

	/** Dash Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DashAction;

protected:
	void OnSetDestinationStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();


	void OnDashAbility();

	virtual void SendAbilityLocalInput(bool Value, int32 InputID);

};
