// SkyRain


#include "AbilitySystem/Abilities/StarHeroGameplayAbility.h"
#include "Characters/StarHeroCharacter.h"
#include "Controllers/StarPlayerController.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "StarGameplayTags.h"

AStarHeroCharacter* UStarHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	// ĳ�õ� ĳ���Ͱ� ���ٸ�? ����.
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
	check(EffectClass);	// GE �� �ִ��� Ȯ��.

	FGameplayEffectContextHandle ContextHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);	// Ability�� ����ϴ� GA(����, ��ų)�� ����.
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());	
	// Instigator(1th Param) : GA�� ����ϴ� ĳ����(�����)
	// EffectCauser(2th Params) : GE�� �߱��ϴ� ĳ���� (�����)
	
	FGameplayEffectSpecHandle EffectSpecHandle = GetStarAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// GameplayTag - amount�� Mapping�ؼ� GE�� �����ϴ� ���
	// Shared_SetByCaller_BaseDamage�� �ش��ϴ� Attribute�� ���� InWeaponBaseDamage�� ����
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		StarGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	if (InCurrentAttackTypeTag.IsValid())
	{
		// ���� �����ϴ� ������ �޺� - InCurrentComboCount �� ���� ����.
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
