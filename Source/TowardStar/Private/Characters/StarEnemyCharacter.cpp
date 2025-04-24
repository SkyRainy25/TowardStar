// SkyRain


#include "Characters/StarEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/StarWidgetBase.h"
#include "Components/BoxComponent.h"
#include "StarFunctionLibrary.h"
#include "GameModes/StarGameModeBase.h"

#include "StarDebugHelper.h"

AStarEnemyCharacter::AStarEnemyCharacter()
{

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");

	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);


	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
}

UPawnCombatComponent* AStarEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AStarEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AStarEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AStarEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UStarWidgetBase* HealthWidget = Cast<UStarWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}

void AStarEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AI가 Possess 하자마자 StartupData 초기화 
	InitEnemyStartUpData();
}

#if WITH_EDITOR
void AStarEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 변경된 Property의 이름이 LeftHandCollisionBoxAttachBoneName라면
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		// CollisionBox를 LeftHandCollisionBoxAttachBoneName에 해당하는 위치에 부착.
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
}
#endif

// 데미지를 처리하는 함수.
void AStarEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// 적대적일 경우
		if (UStarFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);	// 데미지 적용.
		}
	}
}

void AStarEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())	return;

	int32 AbilityApplyLevel = 1;

	if (AStarGameModeBase* StarGameMode = GetWorld()->GetAuthGameMode<AStarGameModeBase>())
	{
		switch (StarGameMode->GetCurrentGameDifficulty())
		{
		case EStarGameDifficulty::Easy:
			AbilityApplyLevel = 1;
			break;
		case EStarGameDifficulty::Normal:
			AbilityApplyLevel = 2;
			break;
		case EStarGameDifficulty::Hard:
			AbilityApplyLevel = 3;
			break;
		case EStarGameDifficulty::VeryHard:
			AbilityApplyLevel = 4;
			break;
		default:
			break;
		}
	}

	// EnemyStartupData를 AsyncLoading 하는 과정.
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this, AbilityApplyLevel]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(StarAbilitySystemComponent, AbilityApplyLevel);
				}
			}
		)
	);
}
