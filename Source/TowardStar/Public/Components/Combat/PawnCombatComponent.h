// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "PawnCombatComponent.generated.h"

class AStarWeaponBase;

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
};
/**
 * 
 */

UCLASS()
class TOWARDSTAR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AStarWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	AStarWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTag) const;

	UPROPERTY(BlueprintReadWrite, Category = "Star|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	AStarWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Star|Combat")
	void ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType = EToggleDamageType::CurrentEquippedWeapon);

	// StarWeaponBase의 콜리전 박스가 Overlap될 때, ExecuteIfBound(델리게이트의 Broadcast기능)를 통해 알리면,
	// 호출할 콜백함수.(Delegate와 바인딩하기 위함)
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

protected:
	virtual void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable);
	virtual void ToggleBodyCollsionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);

	TArray<AActor*> OverlappedActors;

private:
	TMap<FGameplayTag, AStarWeaponBase*> CharacterCarriedWeaponMap;
};
