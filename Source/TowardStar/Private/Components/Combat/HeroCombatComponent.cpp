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
	//HeroWeaponData ����ü( AnimLayer, Ability, Input, Damage �� �����ϴ� ����ü)
	// => �𸮾󿡼� ���� ����.
	return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// Overlap�� ���Ͱ� �̹� ���� ��� => ����
	if (OverlappedActors.Contains(HitActor))	return;

	// ��Ͽ� ���� ��쿡�� �߰�/
	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	// Hit Event�� Notify.
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

