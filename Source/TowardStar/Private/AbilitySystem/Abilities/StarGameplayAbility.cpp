// SkyRain


#include "AbilitySystem/Abilities/StarGameplayAbility.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "StarFunctionLibrary.h"
#include "StarGameplayTags.h"

#include "Components/Combat/PawnCombatComponent.h"

void UStarGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EStarAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UStarGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EStarAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UPawnCombatComponent* UStarGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass< UPawnCombatComponent>();
}

UStarAbilitySystemComponent* UStarGameplayAbility::GetStarAbilitySystemComponentFromActorInfo() const
{
	return Cast<UStarAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UStarGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	check(TargetASC && InSpecHandle.IsValid());

	return GetStarAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC	// Target�� ASC�� ���� GE(������) ����
	);
}

FActiveGameplayEffectHandle UStarGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EStarSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);

	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EStarSuccessType::Successful : EStarSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}

void UStarGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults)
{
	// HitResult�� ���ٸ�? (�ǰ� X) => ����
	if (InHitResults.IsEmpty())	return;

	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());
	// HitResult ��ȸ Ž��
	for (const FHitResult& Hit : InHitResults)
		// HitPawn�� ������ Hostile �Ǵ�
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			if (UStarFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				// NativeApplyEffectSpecHandleToTarget() ������ ����
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				// ���������� ����? => ���� ������ SendGameplayEvenetToActor by using UAbilitySystemBlueprintLibrary 
				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;

					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						StarGameplayTags::Shared_Event_HitReact,
						Data
					);
				}

			}
		}
	}
}
