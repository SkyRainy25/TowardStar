// SkyRain


#include "AbilitySystem/Abilities/StarEnemyGameplayAbility.h"
#include "Characters/StarEnemyCharacter.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "StarGameplayTags.h"

AStarEnemyCharacter* UStarEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedStarEnemyCharacter.IsValid())
	{
		CachedStarEnemyCharacter = Cast<AStarEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedStarEnemyCharacter.IsValid() ? CachedStarEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UStarEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UStarEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
	check(EffectClass);

	// EffectSpecHandle(함수의 Return Value)을 만들기 위한 과정. 
	FGameplayEffectContextHandle ContextHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// GameplayEffectSpecHandle을 만들고
	FGameplayEffectSpecHandle EffectSpecHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// EffectSpecHandle.Data를 통해서 Damage 값을 설정.
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		StarGameplayTags::Shared_SetByCaller_BaseDamage,
		InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
	);

	return EffectSpecHandle;
}
