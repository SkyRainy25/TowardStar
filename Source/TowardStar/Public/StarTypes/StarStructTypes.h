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

	// 무기에 따라 그에 대응하는 Anim을 사용하기 위해 
	// AnimLayer를 사용(에디터에서 무기에 설정한)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UStarHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	// 무기를 장착하기 위한 Input을 Mapping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	// 그 무기가 기지고 있는 Ability를 설정(저장)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FStarHeroAbilitySet> DefaultWeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FStarHeroSpecialAbilitySet> SpecialWeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;

	// 무기 이미지 Texture를 위한 SoftPtr
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};
