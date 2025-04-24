// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpData.generated.h"

class UStarEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UDataAsset_EnemyStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
	
public:
	virtual void GiveToAbilitySystemComponent(UStarAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

private:
	// Enemy Class�� ����� Ability�� ������ ����.
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UStarEnemyGameplayAbility > > EnemyCombatAbilities;
};
