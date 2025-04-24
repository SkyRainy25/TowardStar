// SkyRain


#include "Items/PickUps/StarStoneBase.h"
#include "Characters/StarHeroCharacter.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "StarGameplayTags.h"

void AStarStoneBase::Consume(UStarAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);

	UGameplayEffect* EffectCDO = StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();

	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		EffectCDO,
		ApplyLevel,
		AbilitySystemComponent->MakeEffectContext()
	);

	BP_OnStoneConsumed();
}

void AStarStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AStarHeroCharacter* OverlappedHeroCharacter = Cast<AStarHeroCharacter>(OtherActor))
	{
		OverlappedHeroCharacter->GetStarAbilitySystemComponent()->TryActivateAbilityByTag(StarGameplayTags::Player_Ability_PickUp_Stones);
	}
}
