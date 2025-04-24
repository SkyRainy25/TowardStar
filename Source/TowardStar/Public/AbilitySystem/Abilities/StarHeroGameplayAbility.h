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

	// ������ ������ �ϱ� ���� FGameplayEffectSpecHandle�� ����� �Լ�
	// ������ ��꿡 �ʿ��� �����͸� �����ϰ� ����.
	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	UFUNCTION(BlueprintCallable, Category = "Star|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime);

private:
	// �������� 
	// Reference�� ������ ����. ������ ���� x
	// reference counting�� ������Ű�� ������, ����ִ�(alive)�� ����.
	// �𸮾󿡼� ���� ���.
	// ���� value�� ĳ���ϱ� ���ؼ� ���.
	TWeakObjectPtr<AStarHeroCharacter> CachedStarHeroCharacter;
	TWeakObjectPtr<AStarPlayerController> CachedStarPlayerController;
};
