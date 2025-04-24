// SkyRain


#include "AbilitySystem/Abilities/StarHeroGameplayAbility.h"
#include "Characters/StarHeroCharacter.h"
#include "Controllers/StarPlayerController.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "StarGameplayTags.h"

AStarHeroCharacter* UStarHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	// 캐시된 캐릭터가 없다면? 저장.
	if (!CachedStarHeroCharacter.IsValid())
	{
		CachedStarHeroCharacter = Cast<AStarHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedStarHeroCharacter.IsValid() ? CachedStarHeroCharacter.Get() : nullptr;
}

AStarPlayerController* UStarHeroGameplayAbility::GetStarPlayerControllerFromActorInfo()
{
	if (!CachedStarPlayerController.IsValid())
	{
		CachedStarPlayerController = Cast<AStarPlayerController>(CurrentActorInfo->PlayerController);
	}
	return CachedStarPlayerController.IsValid() ? CachedStarPlayerController.Get() : nullptr;
}

UHeroCombatComponent* UStarHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UHeroUIComponent* UStarHeroGameplayAbility::GetHeroUIComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroUIComponent();
}

FGameplayEffectSpecHandle UStarHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount)
{
	check(EffectClass);	// GE 가 있는지 확인.

	FGameplayEffectContextHandle ContextHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);	// Ability는 사용하는 GA(공격, 스킬)로 설정.
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());	
	// Instigator(1th Param) : GA를 사용하는 캐릭터(사용자)
	// EffectCauser(2th Params) : GE를 야기하는 캐릭터 (사용자)
	
	FGameplayEffectSpecHandle EffectSpecHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// GameplayTag - amount를 Mapping해서 GE를 적용하는 방식
	// Shared_SetByCaller_BaseDamage에 해당하는 Attribute의 값을 InWeaponBaseDamage로 설정
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		StarGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	if (InCurrentAttackTypeTag.IsValid())
	{
		// 현재 공격하는 유형의 콤보 - InCurrentComboCount 로 매핑 설정.
		EffectSpecHandle.Data->SetSetByCallerMagnitude(
			InCurrentAttackTypeTag,
			InUsedComboCount
		);
	}

	return EffectSpecHandle;
}

bool UStarHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime)
{
	check(InCooldownTag.IsValid());

	FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());

	TArray<TPair<float, float>> TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);

	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
		TotalCooldownTime = TimeRemainingAndDuration[0].Value;
	}

	return RemainingCooldownTime > 0.f;
}
