// SkyRain


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUp.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/StarHeroCharacter.h"
#include "Items/PickUps/StarStoneBase.h"
#include "Components/UI/HeroUIComponent.h"

void UHeroGameplayAbility_PickUp::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);   // UI �����ֱ�

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUp::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);   // UI ����

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_PickUp::CollectStones()
{
    CollectedStones.Empty();    // �ʱ�ȭ

	TArray<FHitResult> TraceHits;

	// �ֺ� Stone Ž�� BoxTraceMultiForObjects
    UKismetSystemLibrary::BoxTraceMultiForObjects(
        GetHeroCharacterFromActorInfo(),
        GetHeroCharacterFromActorInfo()->GetActorLocation(),
        GetHeroCharacterFromActorInfo()->GetActorLocation() + -GetHeroCharacterFromActorInfo()->GetActorUpVector() * BoxTraceDistance,
        TraceBoxSize / 2.f,
        (-GetHeroCharacterFromActorInfo()->GetActorUpVector()).ToOrientationRotator(),
        StoneTraceChannel,
        false,
        TArray<AActor*>(),
        bDrawDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
        TraceHits,
        true
    );
   
    // TraceHit�� ����� ���Ϳ��� Stone�� �͸� �߰�.
    for (const FHitResult& TraceHit : TraceHits)
    {
        if (AStarStoneBase* FoundStone = Cast<AStarStoneBase>(TraceHit.GetActor()))
        {
            CollectedStones.AddUnique(FoundStone);
        }
    }

    if (CollectedStones.IsEmpty())
    {
        CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
    }

}

void UHeroGameplayAbility_PickUp::ConsumeStones()
{
    if (CollectedStones.IsEmpty())
    {
        CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
        return;
    }

    // �ִٸ� ��ȸ Ž�� => Consume
    for (AStarStoneBase* CollectedStone : CollectedStones)
    {
        if (CollectedStone)
        {
            CollectedStone->Consume(GetStarAbilitySystemComponentFromActorInfo(), GetAbilityLevel());
        }
    }
}
