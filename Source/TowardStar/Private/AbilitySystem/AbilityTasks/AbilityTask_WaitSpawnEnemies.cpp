// SkyRain


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"
#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "NavigationSystem.h"
#include "Characters/StarEnemyCharacter.h"

#include "StarDebugHelper.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag, TSoftClassPtr<AStarEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn, const FVector& SpawnOrigin, float RandomSpawnRadius)
{
    UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
    Node->CachedEventTag = EventTag;
    Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
    Node->CachedNumToSpawn = NumToSpawn;
    Node->CachedSpawnOrigin = SpawnOrigin;
    Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	
	return Node;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
    // TMap<FGameplayTag, FGameplayEventMulticastDelegate> 에서
    // 해당 Key(GameplayTag)에 해당하는 Valud(FGameplayEventMulticastDelegate) 를 찾는데, 없으면 추가.
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

    // Delegate와 바인딩
    DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

    // 언바인딩
    Delegate.Remove(DelegateHandle);

    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
    // Spawn 해야할 클래스가 있다면? -> RequestAsyncLoad에 필요한 SoftObjectPath를 확인하기 위함.
    if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
    {
        // Asset Manager를 통해서 AsyncLoad
        // RequestAsyncLoad를 통해, 콜백함수와도 바인딩
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
            CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
        );
    }
    else     // 없다면 DidNotSpawn을 통해 Broadcast
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            // 빈 배열을 넘김.
            DidNotSpawn.Broadcast(TArray<AStarEnemyCharacter*>());
        }

        EndTask();
    }


}

// 실제 Spawn이 이루어지는 함수.
void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
    UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
    UWorld* World = GetWorld();

    if (!LoadedClass || !World)
    {
        /** This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.  */
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            // 빈 배열을 넘김.
            DidNotSpawn.Broadcast(TArray<AStarEnemyCharacter*>());
        }

        EndTask();

        return;
    }

    // EnemyCharacter를 담은 배열 선언
    TArray<AStarEnemyCharacter*> SpawnedEnemies;

    FActorSpawnParameters SpawnParam;
    SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 배열 순회 
    for (int32 i = 0; i < CachedNumToSpawn; i++)
    {
        FVector RandomLocation; // OutParam
        // -> 랜덤으로 Spawn할 위치를 UNavigationSystemV1::K2_GetRandomReachablePointInRadius를 통해 가져옴
        UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);

        RandomLocation += FVector(0.f, 0.f, 150.f);

        // 보스가 바라보는 방향의 FRotator 정보를 가져옴.
        const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();
        // 그 위치에 Enemy Spawn.
        AStarEnemyCharacter* SpawnedEnemy = World->SpawnActor<AStarEnemyCharacter>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParam);

        if (SpawnedEnemy)
        {
            SpawnedEnemies.Add(SpawnedEnemy);   // 배열에 추가
        }
    }

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        if (!SpawnedEnemies.IsEmpty())
        {
            OnSpawnFinished.Broadcast(SpawnedEnemies);
        }
        else
        {
            DidNotSpawn.Broadcast(TArray<AStarEnemyCharacter*>());
        }
    }

    EndTask();



}
