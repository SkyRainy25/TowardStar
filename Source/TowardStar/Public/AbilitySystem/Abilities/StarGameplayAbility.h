// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StarGameplayAbility.generated.h"

class UPawnCombatComponent;
class UStarAbilitySystemComponent;

UENUM(BlueprintType)
enum class EStarAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	//~ Begin UGameplayAbility Interface.
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

	UPROPERTY(EditDefaultsOnly, Category = "StarAbility")
	EStarAbilityActivationPolicy AbilityActivationPolicy = EStarAbilityActivationPolicy::OnTriggered;

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Star|Ability")
	UStarAbilitySystemComponent* GetStarAbilitySystemComponentFromActorInfo() const;

	// MakeHeroDamageEffectSpecHandle()�� ���� ���� FGameplayEffectSpecHandle�� ����. GE�� �����ϴ� �Լ�.
	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	// �𸮾󿡼� ����� �� �ְ� ������ �Լ�.
	UFUNCTION(BlueprintCallable, Category = "Star|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EStarSuccessType& OutSuccessType);

	UFUNCTION(BlueprintCallable, Category = "Star|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);
};
