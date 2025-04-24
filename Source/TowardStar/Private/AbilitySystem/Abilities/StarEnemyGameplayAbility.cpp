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

	// EffectSpecHandle(�Լ��� Return Value)�� ����� ���� ����. 
	FGameplayEffectContextHandle ContextHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// GameplayEffectSpecHandle�� �����
	FGameplayEffectSpecHandle EffectSpecHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// EffectSpecHandle.Data�� ���ؼ� Damage ���� ����.
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		StarGameplayTags::Shared_SetByCaller_BaseDamage,
		InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
	);

	return EffectSpecHandle;
}
