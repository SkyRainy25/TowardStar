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

	// ������ ��, AbilitySpecHandle�� �ӽ� ����. => GA_UnEquip�� ����ϱ� ����.
	// GA_Equip���� GiveAbility�� ���� Ȱ��ȭ�� Ability�� Handle��
	// GA_UnEquip���� ClearAbility�� ����ϴµ� �ʿ��� AbilityHandle�� �Ѱ��ֱ� ����
	// (GrantedAbilitySpecHandles)�� ���ؼ�.
	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;
private:
	// ���⸦ ���������� ��, 
	// �� ���Ⱑ ���� Ability�� ClearAbility(EndAbility�� ���)�ϱ� ����
	// �ʿ��� Handle�� �ӽ� �����ϱ� ���� ����.
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
