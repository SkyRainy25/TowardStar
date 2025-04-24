// SkyRain


#include "StarTypes/StarStructTypes.h"
#include "AbilitySystem/Abilities/StarHeroGameplayAbility.h"

bool FStarHeroAbilitySet::IsValid() const
{
    return InputTag.IsValid() && AbilityToGrant;
}
