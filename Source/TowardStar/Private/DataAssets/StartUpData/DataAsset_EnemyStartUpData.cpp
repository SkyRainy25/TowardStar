// SkyRain


#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/StarEnemyGameplayAbility.h"

void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
	
	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf < UStarEnemyGameplayAbility >& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass)	continue;	// Ability �� ���� ���

			// TSubclass�� ���� ������ GameplayAbility�� �̿��Ͽ� FGameplayAbilitySpec�� ����.
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
