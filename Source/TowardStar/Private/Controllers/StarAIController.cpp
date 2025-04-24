// SkyRain


#include "Controllers/StarAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StarDebugHelper.h"

AStarAIController::AStarAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))	// Initializer(함수 실행시 실행)
	// Detour Avoid System(UCrowndFollowingComponent, PathFollowingComponent)을 부모 AIController 클래스로부터 가져와 사용하기 위해 
{


	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;	// 적 감지 O
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;	// 동맹감지 X
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;	// 중립 감지 X
	AISenseConfig_Sight->SightRadius = 5000.f;	// 시야 반경
	AISenseConfig_Sight->LoseSightRadius = 0.f;	// 플레이어와의 거리 > LoseSightRadius => 플레이어 잊기
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;	// 시야각

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);	
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	// Enemy(Player)가 적대적(Hostile)이라면, 콜백함수 호출.

	/* 
	TargetActor가 인식되면 Callback 함수 호출
	=> BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
	를 통해, TargetActor를 설정.
	*/
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	// EnemyClass는 TeamID = 1/ Player = 9;
	SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type AStarAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent =  Cast<IGenericTeamAgentInterface>(PawnToCheck->GetController());

	// OtherTeamAgent = Player
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		// 서로 TeamID가 다를 경우 => 적대적
		return ETeamAttitude::Hostile;
	}
	// 아닐 경우 Friendly
	return ETeamAttitude::Friendly;
}

void AStarAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		// bEnableDetourCrowdAvoidance 설정을 사용하는 경우에만, 실행.
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);

		// 언리얼 에디터에서 설정한 값에 따라서, Quaility 조정.
		switch (DetourCrowdAvoidanceQuality)
		{
		case 1: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);    break;
		case 2: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
		case 3: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);   break;
		case 4: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);   break;
		default:
			break;
		}

		CrowdComp->SetAvoidanceGroup(1);	// 1 팀(Enemy)에 Avoidance 적용
		CrowdComp->SetGroupsToAvoid(1);		// 1 팀을 Avoid하도록
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);	// CollisionQueryRange만큼 피하기?
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
		// 블랙보드 키에서 Target이 설정되지 않을 때만
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			// 블랙보드가 있다면, Object타입으로 값설정.
			// 블랙보드의 "TargetActor"라는 블략보드의 키 Value(값)을 Actor로 설정.
			BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
		}
	}
}

