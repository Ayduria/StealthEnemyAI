// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthEnemyAIController.h"

#include "BlackboardKeys.h"
#include "StealthEnemyAICharacter.h"
#include "StimuliTags.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"

AStealthEnemyAIController::AStealthEnemyAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));

	SetupPerceptionSystem();
}

void AStealthEnemyAIController::SetupPerceptionSystem()
{
	// Create and initialize SightConfig;
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

		SightConfig->SightRadius = 500.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 50.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		// Add sight configuration component to perception component
		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->ConfigureSense(*SightConfig);

		SightID = SightConfig->GetSenseID();
	}

	// Create and initialize hearing config
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	if (HearingConfig)
	{
		HearingConfig->HearingRange = 1000.0f;
		HearingConfig->SetMaxAge(5.0f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

		// Add hearing configuration component to perception component
		GetPerceptionComponent()->ConfigureSense(*HearingConfig);

		HearingID = HearingConfig->GetSenseID();
	}

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AStealthEnemyAIController::OnTargetDetected);
}

void AStealthEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AStealthEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AStealthEnemyAICharacter* EnemyPawn = Cast<AStealthEnemyAICharacter>(InPawn);
	checkf(EnemyPawn, TEXT("Controller is not possessing a pawn"));

	ControlledPawn = EnemyPawn;
	GetBlackboard()->SetValueAsObject(BBKeys::SelfActor, Cast<UObject>(ControlledPawn));

	// Initialize blackboard and start behavior tree
	BlackboardComponent->InitializeBlackboard(*ControlledPawn->BehaviorTree->BlackboardAsset);
	BehaviorTreeComponent->StartTree(*ControlledPawn->BehaviorTree);

	// Set base values in blackboard
	GetBlackboard()->SetValueAsEnum(BBKeys::Behavior, static_cast<uint8>(ControlledPawn->GetBehavior()));

	// If enemy has Stationary behavior, set Guarding state and save base position and rotation
	if (IsEnemyStationary())
	{
		BaseState = EEnemyState::Guarding;
		BasePosition = ControlledPawn->GetActorLocation();
		BaseRotation = ControlledPawn->GetActorRotation();
	} else // If enemy has Patrol behavior, set Patrolling state
	{
		BaseState = EEnemyState::Patrolling;
	}

	GetBlackboard()->SetValueAsEnum(BBKeys::State, static_cast<uint8>(BaseState));

	ControlledPawn->SetSpeedToNormal();
	ControlledPawn->Multicast_SetConeMaterial(DefaultConeMaterial);
}

void AStealthEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	// If stimulus is entering enemy perception
	if (Stimulus.WasSuccessfullySensed())
	{
		// If stimulus is coming from another enemy, return
		if (auto const DetectedActor = Cast<AStealthEnemyAICharacter>(Actor); IsValid(DetectedActor)) return;

		// If stimulus is noise, set suspicious
		// Otherwise, stimulus is sight and chase is triggered
		if (Stimulus.Type == HearingID)
			HeardNoise(Stimulus);
		else if (Stimulus.Type == SightID)
			StartChase(Cast<ACharacter>(Actor));
	}
}

UBlackboardComponent* AStealthEnemyAIController::GetBlackboard() const
{
	return BlackboardComponent;
}

int AStealthEnemyAIController::GetPatrolIndex() const
{
	return WaypointIndex;
}

void AStealthEnemyAIController::SetPatrolIndex(const int Index)
{
	WaypointIndex = Index;
}

bool AStealthEnemyAIController::IsReversingPatrol() const
{
	return bReversingPatrol;
}

void AStealthEnemyAIController::ToggleReversePatrol()
{
	bReversingPatrol = !bReversingPatrol;
}

void AStealthEnemyAIController::FaceCorrectDirection(const FRotator Rotation)
{
	SetControlRotation(Rotation);
	ControlledPawn->SetActorRotation(Rotation);
}

void AStealthEnemyAIController::StartChase(ACharacter* Player)
{
	InvestigateTimerHandle.Invalidate();

	SetState(EEnemyState::Chasing);
	SetTargetPlayer(Player);
	ControlledPawn->SetSpeedToChase();
	ControlledPawn->Multicast_SetConeMaterial(ChaseConeMaterial);
}

void AStealthEnemyAIController::StartInvestigating()
{
	SetState(EEnemyState::Investigating);
	InvestigateTimerHandle.Invalidate();
	GetWorldTimerManager().SetTimer(InvestigateTimerHandle, this, &AStealthEnemyAIController::ResumeActivities, InvestigatingDuration, false);
}

FVector AStealthEnemyAIController::GetNextWaypointLocation() const
{
	return IsEnemyStationary() ? FVector::ZeroVector : GetBlackboard()->GetValueAsVector(BBKeys::NextWaypointLocation);
}

FVector AStealthEnemyAIController::GetBasePosition() const
{
	return BasePosition;
}

FRotator AStealthEnemyAIController::GetBaseRotation() const
{
	return BaseRotation;
}

bool AStealthEnemyAIController::IsEnemyStationary() const
{
	return ControlledPawn->GetBehavior() == EEnemyBehavior::Stationary;
}

bool AStealthEnemyAIController::IsSuspicious() const
{
	return ControlledPawn->GetState() == EEnemyState::Suspicious;
}

bool AStealthEnemyAIController::IsInvestigating() const
{
	return ControlledPawn->GetState() == EEnemyState::Investigating;
}

void AStealthEnemyAIController::HeardNoise(const FAIStimulus Stimulus)
{
	SetTargetLocation(Stimulus.StimulusLocation);

	if (IsSuspicious() || IsInvestigating()) return;

	SetSuspicious(Stimulus.Tag);
}

void AStealthEnemyAIController::SetSuspicious(const FName NoiseTag)
{
	SetState(EEnemyState::Suspicious);
	ControlledPawn->Multicast_SetConeMaterial(SusConeMaterial);

	// Start investigation after timer
	const float InvestigateDelay = NoiseTag == NoiseTags::QuietStepNoise ? 0.1f : DelayBeforeInvestigating;
	InvestigateTimerHandle.Invalidate();
	GetWorldTimerManager().SetTimer(InvestigateTimerHandle, this, &AStealthEnemyAIController::StartInvestigating, InvestigateDelay, false);
}

void AStealthEnemyAIController::ResumeActivities()
{
	InvestigateTimerHandle.Invalidate();

	// Reset enemy to default state
	SetState(EEnemyState::Resuming);
	SetTargetPlayer(nullptr);
	IsEnemyStationary() ? SetTargetLocation(BasePosition) : SetTargetLocation(GetNextWaypointLocation());
	ControlledPawn->SetSpeedToNormal();
	ControlledPawn->Multicast_SetConeMaterial(DefaultConeMaterial);
}

void AStealthEnemyAIController::SetState(EEnemyState State) const
{
	ControlledPawn->SetState(State);
	GetBlackboard()->SetValueAsEnum(BBKeys::State, static_cast<uint8>(State));
}

void AStealthEnemyAIController::SetTargetActor(AActor* Target) const
{
	GetBlackboard()->SetValueAsObject(BBKeys::TargetActor, Target);
}

void AStealthEnemyAIController::SetTargetPlayer(ACharacter* Player) const
{
	SetTargetActor(Cast<AActor>(Player));
	GetBlackboard()->SetValueAsObject(BBKeys::TargetPlayer, Player);
}

void AStealthEnemyAIController::SetTargetLocation(const FVector Location) const
{
	GetBlackboard()->SetValueAsVector(BBKeys::TargetLocation, Location);
}

void AStealthEnemyAIController::SetNextWaypointLocation(const FVector Location) const
{
	if (!IsEnemyStationary())
	{
		GetBlackboard()->SetValueAsVector(BBKeys::NextWaypointLocation, Location);
	}
}
