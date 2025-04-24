// SkyRain


#include "Characters/StarBaseCharacter.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/StarAttributeSet.h"
#include "MotionWarpingComponent.h"

// Sets default values
AStarBaseCharacter::AStarBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	StarAbilitySystemComponent = CreateDefaultSubobject<UStarAbilitySystemComponent>(TEXT("StarAbilitySystemComponent"));

	StarAttributeSet = CreateDefaultSubobject<UStarAttributeSet>(TEXT("StarAttributeSet"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

UAbilitySystemComponent* AStarBaseCharacter::GetAbilitySystemComponent() const
{
	return GetStarAbilitySystemComponent();
}

UPawnCombatComponent* AStarBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* AStarBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

void AStarBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (StarAbilitySystemComponent)
	{
		// OwnerActor , AvatarActor
		// ForDreamAdventure ������ ���� PS , Character ������
		// �� ������Ʈ������ StarHeroCharacter�� �� �� �ش�.
		StarAbilitySystemComponent->InitAbilityActorInfo(this, this);

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());

	}
}

