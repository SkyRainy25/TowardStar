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
			if (!AbilityClass)	continue;	// Ability 가 없는 경우

			// TSubclass를 통해 설정된 GameplayAbility를 이용하여 FGameplayAbilitySpec을 생성.
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
