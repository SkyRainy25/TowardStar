// SkyRain

#pragma once

#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "StarStructTypes.generated.h"

class UStarHeroLinkedAnimLayer;
class UStarHeroGameplayAbility;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FStarHeroAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UStarHeroGameplayAbility> AbilityToGrant;

	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FStarHeroSpecialAbilitySet : public FStarHeroAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> SoftAbilityIconMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Player.Cooldown"))
	FGameplayTag AbilityCooldownTag;
};

USTRUCT(BlueprintType)
struct FStarHeroWeaponData
{
	GENERATED_BODY()

	// ���⿡ ���� �׿� �����ϴ� Anim�� ����ϱ� ���� 
	// AnimLayer�� ���(�����Ϳ��� ���⿡ ������)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UStarHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	// ���⸦ �����ϱ� ���� Input�� Mapping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	// �� ���Ⱑ ������ �ִ� Ability�� ����(����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FStarHeroAbilitySet> DefaultWeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FStarHeroSpecialAbilitySet> SpecialWeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;

	// ���� �̹��� Texture�� ���� SoftPtr
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};
