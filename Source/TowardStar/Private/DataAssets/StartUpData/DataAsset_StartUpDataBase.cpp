// SkyRain


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/StarGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	check(InASCToGive);

	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			InASCToGive->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASCToGive->MakeEffectContext()
			);
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UStarGameplayAbility>>& InAbilitiesToGive, UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())	return;

	for (const TSubclassOf<UStarGameplayAbility > &Ability : InAbilitiesToGive)
	{
		if (!Ability)	continue;

		// TSubclassOf<UStarGameplayAbility> 를 통해서 GameplayAbilitySpec을 생성
		FGameplayAbilitySpec AbilitySpec(Ability);
		// 세부 설정
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InASCToGive->GiveAbility(AbilitySpec);
	}
}
