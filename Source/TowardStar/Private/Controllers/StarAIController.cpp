// SkyRain


#include "Controllers/StarAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StarDebugHelper.h"

AStarAIController::AStarAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))	// Initializer(�Լ� ����� ����)
	// Detour Avoid System(UCrowndFollowingComponent, PathFollowingComponent)�� �θ� AIController Ŭ�����κ��� ������ ����ϱ� ���� 
{


	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;	// �� ���� O
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;	// ���Ͱ��� X
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;	// �߸� ���� X
	AISenseConfig_Sight->SightRadius = 5000.f;	// �þ� �ݰ�
	AISenseConfig_Sight->LoseSightRadius = 0.f;	// �÷��̾���� �Ÿ� > LoseSightRadius => �÷��̾� �ر�
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;	// �þ߰�

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);	
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	// Enemy(Player)�� ������(Hostile)�̶��, �ݹ��Լ� ȣ��.

	/* 
	TargetActor�� �νĵǸ� Callback �Լ� ȣ��
	=> BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
	�� ����, TargetActor�� ����.
	*/
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	// EnemyClass�� TeamID = 1/ Player = 9;
	SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type AStarAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent =  Cast<IGenericTeamAgentInterface>(PawnToCheck->GetController());

	// OtherTeamAgent = Player
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		// ���� TeamID�� �ٸ� ��� => ������
		return ETeamAttitude::Hostile;
	}
	// �ƴ� ��� Friendly
	return ETeamAttitude::Friendly;
}

void AStarAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		// bEnableDetourCrowdAvoidance ������ ����ϴ� ��쿡��, ����.
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);

		// �𸮾� �����Ϳ��� ������ ���� ����, Quaility ����.
		switch (DetourCrowdAvoidanceQuality)
		{
		case 1: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);    break;
		case 2: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
		case 3: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);   break;
		case 4: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);   break;
		default:
			break;
		}

		CrowdComp->SetAvoidanceGroup(1);	// 1 ��(Enemy)�� Avoidance ����
		CrowdComp->SetGroupsToAvoid(1);		// 1 ���� Avoid�ϵ���
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);	// CollisionQueryRange��ŭ ���ϱ�?
	}
}

/**
 * Notifies all bound objects that perception info has been updated for a given target.
 * The notification is broadcasted for any received stimulus or on change of state
 * according to the stimulus configuration.
 *
 * Note - This delegate will not be called if source actor is no longer valid
 * by the time a stimulus gets processed.
 * Use OnTargetPerceptionInfoUpdated providing a source id to handle those cases.
 *
 * @param	SourceActor	Actor associated to the stimulus (can not be null)
 * @param	Stimulus	Updated stimulus
 */
void AStarAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		// ������ Ű���� Target�� �������� ���� ����
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			// �����尡 �ִٸ�, ObjectŸ������ ������.
			// �������� "TargetActor"��� �������� Ű Value(��)�� Actor�� ����.
			BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
		}
	}
}

