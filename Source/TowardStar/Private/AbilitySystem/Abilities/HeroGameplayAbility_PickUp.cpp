// SkyRain


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUp.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/StarHeroCharacter.h"
#include "Items/PickUps/StarStoneBase.h"
#include "Components/UI/HeroUIComponent.h"

void UHeroGameplayAbility_PickUp::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);   // UI 보여주기

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUp::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);   // UI 제거

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_PickUp::CollectStones()
{
    CollectedStones.Empty();    // 초기화

	TArray<FHitResult> TraceHits;

	// 주변 Stone 탐색 BoxTraceMultiForObjects
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
   
    // TraceHit에 얻어진 액터에서 Stone인 것만 추가.
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

    // 있다면 순회 탐색 => Consume
    for (AStarStoneBase* CollectedStone : CollectedStones)
    {
        if (CollectedStone)
        {
            CollectedStone->Consume(GetStarAbilitySystemComponentFromActorInfo(), GetAbilityLevel());
        }
    }
}
