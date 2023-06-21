// Fill out your copyright notice in the Description page of Project Settings.


//#include "GameplayAbility/AbilityTask/TDSLAT_WaitPlayerMoveToLocation.h"
//#include "GameFramework/Character.h"
//#include "GameFramework/CharacterMovementComponent.h"
//
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(TDSLAT_WaitPlayerMoveToLocation)
//
//UTDSLAT_WaitPlayerMoveToLocation::UTDSLAT_WaitPlayerMoveToLocation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
//{
//}
//
//void UTDSLAT_WaitPlayerMoveToLocation::OnPlayerReachedCallback()
//{
//	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
//	if (Character && !Character->GetCharacterMovement()->IsMovingOnGround())
//	{
//		// Character has stopped moving
//		// Perform actions here
//		UE_LOG(LogTemp, Warning, TEXT("Character has stopped moving!"));
//	}
//}
//
//UTDSLAT_WaitPlayerMoveToLocation* UTDSLAT_WaitPlayerMoveToLocation::AbilityTaskWaitPlayerMoveToLocation(UGameplayAbility* OwningAbility, FName TaskInstanceName)
//{
//	UTDSLAT_WaitPlayerMoveToLocation* MyObj = NewAbilityTask<UTDSLAT_WaitPlayerMoveToLocation>(OwningAbility);
//	return MyObj;
//}
//
//void UTDSLAT_WaitPlayerMoveToLocation::Activate()
//{
//	Super::Activate();
//
//	SetWaitingOnAvatar();
//
//	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
//	if (!Character)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("AT_WaitLoc Character"));
//		return;
//	}
//	MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
//	
//	if (MovementComponent)
//	{
//
//	}
//}
//
//void UTDSLAT_WaitPlayerMoveToLocation::OnDestroy(bool AbilityEnded)
//{
//
//
//	Super::OnDestroy(AbilityEnded);
//}
