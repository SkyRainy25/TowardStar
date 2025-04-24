// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/StarGameplayAbility.h"
#include "StarTypes/StarEnumTypes.h"
#include "StarHeroGameplayAbility.generated.h"

class AStarHeroCharacter;
class AStarPlayerController;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarHeroGameplayAbility : public UStarGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	AStarHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	AStarPlayerController* GetStarPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	UHeroUIComponent* GetHeroUIComponentFromActorInfo();

	// 데미지 적용을 하기 위한 FGameplayEffectSpecHandle을 만드는 함수
	// 데미지 계산에 필요한 데이터를 설정하고 저장.
	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	UFUNCTION(BlueprintCallable, Category = "Star|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime);

private:
	// 약한참조 
	// Reference만 가지고 있음. 포인터 추적 x
	// reference counting을 증가시키지 않으며, 살아있는(alive)로 유지.
	// 언리얼에서 자주 사용.
	// 보통 value를 캐시하기 위해서 사용.
	TWeakObjectPtr<AStarHeroCharacter> CachedStarHeroCharacter;
	TWeakObjectPtr<AStarPlayerController> CachedStarPlayerController;
};
