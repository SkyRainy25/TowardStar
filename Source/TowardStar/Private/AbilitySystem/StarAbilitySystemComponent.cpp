// SkyRain


#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/StarHeroGameplayAbility.h"
#include "StarGameplayTags.h"

void UStarAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())	return;

	// 0 InputConfig���� InputTag�� Action�� ����(������)
	// 1 InputComp���� BindAbilityInputAction/ BindNativeInputAction(���ø� �Լ�)�� ���
	// => InputComp���� InputTag�� CallbackFunc �� ���� HeroCharacter�� �Ѱ���
	// 2 HeroCharacter���� ASC�� �̿��� InputTag�� �Ѱ��ְ� Ȱ��ȭ.

	// Ȱ��ȭ ������ Ability�� ��ȸ
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// InputTag�� �ش��ϴ� Ability�� ���� ��� Ȱ��ȭ.
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		
		// ��Ŭ�� �̹� Ȱ��ȭ?
		if (InInputTag.MatchesTag(StarGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);	// ���Ű�� �ƴ� ��� �׳� Ȱ��ȭ.
		}

	}
}

// ���콺�� Ŭ���ϰ� ���� ��Ȳ���� ����Ǵ� �Լ�.
void UStarAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(StarGameplayTags::InputTag_MustBeHeld))
		return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// �ش� InInputTag�� ���� Ability�� Ȱ��ȭ ���� + Ability�� �̹� ����ǰ� ������?
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);	// Ability ���
		}
	}
}

void UStarAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FStarHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FStarHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	// ���⿡ Ability�� �������� ���� ��� 
	if (InDefaultWeaponAbilities.IsEmpty())	return;

	for (const FStarHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid())	continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		// EndAbility�� �� ��
		// ClearAbility() �Լ��� ����ϰ� �Ǵµ�, �׋� FGameplayAbilitySpecHandle�� �ʿ���
		// => �����صξ�� ��. 
		// GiveAbility�� Return�ϴ� Data ������ FGameplayAbilitySpecHandle.
		// OutGrantedAbilitySpecHandles�� Handle ����.
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}

	for (const FStarHeroSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid())	continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		// EndAbility�� �� ��
		// ClearAbility() �Լ��� ����ϰ� �Ǵµ�, �׋� FGameplayAbilitySpecHandle�� �ʿ���
		// => �����صξ�� ��. 
		// GiveAbility�� Return�ϴ� Data ������ FGameplayAbilitySpecHandle.
		// OutGrantedAbilitySpecHandles�� Handle ����.
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UStarAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())	return;

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		// ��ȿ�� SpecHandle( = Ȱ��ȭ�� Ability��) Clear
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	// �ӽ����忡 ����� �迭 �ʱ�ȭ(�����ֱ�)
	InSpecHandlesToRemove.Empty();
}

bool UStarAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;	// OutParams
	// Tag(TagContainer)�� ���� ��� Acticatable Abilities�� ������ FoundAbilitySpecs(OutParams)�� ����.
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())	// ������� �ʴٸ�
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		// Ȱ��ȭ ���� ���� Ability���
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}
