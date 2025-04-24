// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"

class AStarHeroWeapon;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	AStarHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;

	// Player의 Weapon을 가져오는 Helper Func
	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	AStarHeroWeapon* GetHeroCurrentEquippedWeapon() const;

	// WeaponDamage를 가져오는 함수.
	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	float GetHeroCurrentEquippWeaponDamageAtLevel(float InLevel) const;

	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
};
