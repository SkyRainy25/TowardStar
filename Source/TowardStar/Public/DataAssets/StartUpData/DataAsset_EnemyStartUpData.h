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
	// Enemy Class가 사용할 Ability를 설정할 변수.
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UStarEnemyGameplayAbility > > EnemyCombatAbilities;
};
