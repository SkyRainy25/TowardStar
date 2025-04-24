// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/StarWeaponBase.h"
#include "StarTypes/StarStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "StarHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTAR_API AStarHeroWeapon : public AStarWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FStarHeroWeaponData HeroWeaponData;

	// 장착할 때, AbilitySpecHandle을 임시 저장. => GA_UnEquip에 사용하기 위함.
	// GA_Equip에서 GiveAbility를 통해 활성화한 Ability의 Handle을
	// GA_UnEquip에서 ClearAbility에 사용하는데 필요한 AbilityHandle을 넘겨주기 위함
	// (GrantedAbilitySpecHandles)를 통해서.
	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;
private:
	// 무기를 장착해제할 떄, 
	// 그 무기가 가진 Ability를 ClearAbility(EndAbility에 사용)하기 위해
	// 필요한 Handle을 임시 저장하기 위한 변수.
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
