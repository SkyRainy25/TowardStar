// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "StarTypes/StarStructTypes.h"
#include "StarAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "Star|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FStarHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FStarHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	// Unreal에서 Output Parameter를 보여주기 위한 매크로
	// => UPARAM(ref)
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemovedGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
