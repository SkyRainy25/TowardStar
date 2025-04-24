// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UStarGameplayAbility;
class UStarAbilitySystemComponent;

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
	
public: 
	virtual void GiveToAbilitySystemComponent(UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UStarGameplayAbility>> ActivateOnGivenAbilities;

	// HitReact나 Death 같은 Ability를 위함
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UStarGameplayAbility>> ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;
	
	void GrantAbilities(const TArray<TSubclassOf<UStarGameplayAbility>>& InAbilitiesToGive, UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};
