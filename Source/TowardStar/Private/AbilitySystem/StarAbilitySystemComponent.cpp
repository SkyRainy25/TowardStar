// SkyRain


#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/StarHeroGameplayAbility.h"
#include "StarGameplayTags.h"

void UStarAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())	return;

	// 0 InputConfig에서 InputTag와 Action을 매핑(에디터)
	// 1 InputComp에서 BindAbilityInputAction/ BindNativeInputAction(템플릿 함수)를 사용
	// => InputComp에서 InputTag를 CallbackFunc 을 통해 HeroCharacter로 넘겨줌
	// 2 HeroCharacter에서 ASC를 이용해 InputTag를 넘겨주고 활성화.

	// 활성화 가능한 Ability를 순회
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// InputTag에 해당하는 Ability가 있을 경우 활성화.
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		
		// 토클이 이미 활성화?
		if (InInputTag.MatchesTag(StarGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);	// 토글키가 아닐 경우 그냥 활성화.
		}

	}
}

// 마우스를 클릭하고 뗐을 상황에서 실행되는 함수.
void UStarAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(StarGameplayTags::InputTag_MustBeHeld))
		return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 해당 InInputTag를 가진 Ability가 활성화 상태 + Ability가 이미 실행되고 있으면?
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);	// Ability 취소
		}
	}
}

void UStarAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FStarHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FStarHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	// 무기에 Ability가 존재하지 않을 경우 
	if (InDefaultWeaponAbilities.IsEmpty())	return;

	for (const FStarHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid())	continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		// EndAbility를 할 때
		// ClearAbility() 함수를 사용하게 되는데, 그떄 FGameplayAbilitySpecHandle이 필요함
		// => 저장해두어야 함. 
		// GiveAbility의 Return하는 Data 형식이 FGameplayAbilitySpecHandle.
		// OutGrantedAbilitySpecHandles에 Handle 저장.
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}

	for (const FStarHeroSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid())	continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		// EndAbility를 할 때
		// ClearAbility() 함수를 사용하게 되는데, 그떄 FGameplayAbilitySpecHandle이 필요함
		// => 저장해두어야 함. 
		// GiveAbility의 Return하는 Data 형식이 FGameplayAbilitySpecHandle.
		// OutGrantedAbilitySpecHandles에 Handle 저장.
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UStarAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())	return;

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		// 유효한 SpecHandle( = 활성화된 Ability만) Clear
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	// 임시저장에 사용한 배열 초기화(지워주기)
	InSpecHandlesToRemove.Empty();
}

bool UStarAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;	// OutParams
	// Tag(TagContainer)를 가진 모든 Acticatable Abilities를 가져와 FoundAbilitySpecs(OutParams)에 저장.
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())	// 비어있지 않다면
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		// 활성화 되지 않은 Ability라면
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}
