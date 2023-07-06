// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TDSLEnemyAIController.h"
#include "UObject/ConstructorHelpers.h"
//#include "EngineGlobals.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AI/BlackBoardKeys.h"

#include "Characters/TDSLPlayerCharacter.h"

ATDSLEnemyAIController::ATDSLEnemyAIController(FObjectInitializer const& object_initializer)
{
	bWantsPlayerState = true;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree' Game/MyContent/AI/BT_EnemyAI.BT_EnemyAI'"));
	if (BTObject.Succeeded())
	{
		BT = BTObject.Object;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BT completed!"));
	}
	BTComp = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BB = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	SetPerceptionSystem();
}

void ATDSLEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BT);
	BTComp->StartTree(*BT);
}

void ATDSLEnemyAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	//if (BB)
	//{
	//	BB->InitializeBlackboard(*BT->BlackboardAsset);
	//}
}

UBlackboardComponent* ATDSLEnemyAIController::GetBlackboard() const
{
	return BB;
}

void ATDSLEnemyAIController::OnUpdated(TArray<AActor*> const& updated_actors)
{
}

void ATDSLEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (Stimulus.WasSuccessfullySensed() && Cast<ATDSLPlayerCharacter>(Actor))
	{
		BB->SetValueAsObject(BBKeys::TargetActor, Actor);
		BB->SetValueAsEnum(BBKeys::AIState, StaticCast<int32>(EEnemyAIState::Chase));
	}
}

void ATDSLEnemyAIController::SetPerceptionSystem()
{
	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = AILastSeenLocation;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ATDSLEnemyAIController::OnTargetDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}
