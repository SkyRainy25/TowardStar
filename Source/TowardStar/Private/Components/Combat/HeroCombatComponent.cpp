// SkyRain


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/StarHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "StarGameplayTags.h"

#include "StarDebugHelper.h"

AStarHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast< AStarHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

AStarHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	return Cast<AStarHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippWeaponDamageAtLevel(float InLevel) const
{
	//HeroWeaponData 구조체( AnimLayer, Ability, Input, Damage 를 저장하는 구조체)
	// => 언리얼에서 세부 설정.
	return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// Overlap된 액터가 이미 있을 경우 => 종료
	if (OverlappedActors.Contains(HitActor))	return;

	// 목록에 없을 경우에만 추가/
	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	// Hit Event를 Notify.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		StarGameplayTags::Shared_Event_MeleeHit,
		Data
	);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		StarGameplayTags::Player_Event_HitPause,
		Data
	);
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		StarGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
}

