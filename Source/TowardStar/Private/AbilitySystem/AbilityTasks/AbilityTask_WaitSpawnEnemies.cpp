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
    // TMap<FGameplayTag, FGameplayEventMulticastDelegate> ����
    // �ش� Key(GameplayTag)�� �ش��ϴ� Valud(FGameplayEventMulticastDelegate) �� ã�µ�, ������ �߰�.
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

    // Delegate�� ���ε�
    DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

    // ����ε�
    Delegate.Remove(DelegateHandle);

    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
    // Spawn �ؾ��� Ŭ������ �ִٸ�? -> RequestAsyncLoad�� �ʿ��� SoftObjectPath�� Ȯ���ϱ� ����.
    if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
    {
        // Asset Manager�� ���ؼ� AsyncLoad
        // RequestAsyncLoad�� ����, �ݹ��Լ��͵� ���ε�
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
            CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
        );
    }
    else     // ���ٸ� DidNotSpawn�� ���� Broadcast
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            // �� �迭�� �ѱ�.
            DidNotSpawn.Broadcast(TArray<AStarEnemyCharacter*>());
        }

        EndTask();
    }


}

// ���� Spawn�� �̷������ �Լ�.
void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
    UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
    UWorld* World = GetWorld();

    if (!LoadedClass || !World)
    {
        /** This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.  */
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            // �� �迭�� �ѱ�.
            DidNotSpawn.Broadcast(TArray<AStarEnemyCharacter*>());
        }

        EndTask();

        return;
    }

    // EnemyCharacter�� ���� �迭 ����
    TArray<AStarEnemyCharacter*> SpawnedEnemies;

    FActorSpawnParameters SpawnParam;
    SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // �迭 ��ȸ 
    for (int32 i = 0; i < CachedNumToSpawn; i++)
    {
        FVector RandomLocation; // OutParam
        // -> �������� Spawn�� ��ġ�� UNavigationSystemV1::K2_GetRandomReachablePointInRadius�� ���� ������
        UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);

        RandomLocation += FVector(0.f, 0.f, 150.f);

        // ������ �ٶ󺸴� ������ FRotator ������ ������.
        const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();
        // �� ��ġ�� Enemy Spawn.
        AStarEnemyCharacter* SpawnedEnemy = World->SpawnActor<AStarEnemyCharacter>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParam);

        if (SpawnedEnemy)
        {
            SpawnedEnemies.Add(SpawnedEnemy);   // �迭�� �߰�
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
