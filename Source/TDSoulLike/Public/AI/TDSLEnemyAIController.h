// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "TDSLEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TDSOULLIKE_API ATDSLEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ATDSLEnemyAIController(FObjectInitializer const& object_initializer);
	
protected:
	virtual void BeginPlay() override;
	void OnPossess(APawn* pawn) override;

	class UBlackboardComponent* GetBlackboard() const;

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BTComp;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BT;

	class UBlackboardComponent* BB;
	class UAISenseConfig_Sight* SightConfig;

public:
	UFUNCTION()
	void OnUpdated(TArray<AActor*> const& updated_actors);
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
	UFUNCTION()
	void SetPerceptionSystem();

	//AI Perception
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AISightRadius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AILoseSightRadius = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIFieldOfView = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AISightAge = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AILastSeenLocation = 900.f;

};
