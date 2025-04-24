// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StarTypes/StarEnumTypes.h"
#include "StarGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTAR_API AStarGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AStarGameModeBase();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	EStarGameDifficulty CurrentGameDifficulty;

public:
	FORCEINLINE EStarGameDifficulty GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
};
