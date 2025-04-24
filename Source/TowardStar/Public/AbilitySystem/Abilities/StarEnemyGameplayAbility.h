// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/StarGameplayAbility.h"
#include "StarEnemyGameplayAbility.generated.h"

class AStarrEnemyCharacter;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarEnemyGameplayAbility : public UStarGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	AStarEnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);
private:
	TWeakObjectPtr<AStarEnemyCharacter> CachedStarEnemyCharacter;
};
