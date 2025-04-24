// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "StarTypes/StarEnumTypes.h"
#include "StarSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EStarGameDifficulty SavedCurrentGameDifficulty;
};
