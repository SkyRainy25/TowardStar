// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

class AStarEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate, const TArray<AStarEnemyCharacter*>&, SpawnedEnemies);
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Star|AbilityTasks", meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", NumToSpawn = "1", RandomSpawnRadius = "200"))
    static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
        UGameplayAbility* OwningAbility,
        FGameplayTag EventTag,
        TSoftClassPtr<AStarEnemyCharacter> SoftEnemyClassToSpawn,
        int32 NumToSpawn,
        const FVector& SpawnOrigin,
        float RandomSpawnRadius
    );

    UPROPERTY(BlueprintAssignable)
    FWaitSpawnEnemiesDelegate OnSpawnFinished;

    UPROPERTY(BlueprintAssignable)
    FWaitSpawnEnemiesDelegate DidNotSpawn;

    //~ Begin UGameplayTask Interface
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    //~ End UGameplayTask Interface

private:
    FGameplayTag CachedEventTag;
    TSoftClassPtr<AStarEnemyCharacter> CachedSoftEnemyClassToSpawn;
    int32 CachedNumToSpawn;
    FVector CachedSpawnOrigin;
    float CachedRandomSpawnRadius;

    FDelegateHandle DelegateHandle;

    void OnGameplayEventReceived(const FGameplayEventData* InPayload);
    void OnEnemyClassLoaded();

};
