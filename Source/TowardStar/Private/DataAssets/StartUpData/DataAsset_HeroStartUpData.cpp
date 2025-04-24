// SkyRain


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"

#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/StarHeroGameplayAbility.h"

void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    for (const FStarHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
        if (!AbilitySet.IsValid())   continue;

        /** Version that takes an ability class */
        //FGameplayAbilitySpec(TSubclassOf<UGameplayAbility> InAbilityClass, int32 InLevel = 1, int32 InInputID = INDEX_NONE, UObject * InSourceObject = nullptr);

        FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);  // ������ �ٽ� ����
        AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
        AbilitySpec.Level = ApplyLevel;
        //DynamicAbilityTags : Ability - InputTag�� ����.
        // InputTag�� ���� Ability ��ȯ ����.
        AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
        
        InASCToGive->GiveAbility(AbilitySpec);
    }
}
