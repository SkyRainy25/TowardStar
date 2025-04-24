// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/StarBaseAnimInstance.h"
#include "StarHeroLinkedAnimLayer.generated.h"

class UStarHeroAnimInstance;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarHeroLinkedAnimLayer : public UStarBaseAnimInstance
{
	GENERATED_BODY()

public:
	// Property�� access �ϱ� ���ؼ� ����.
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UStarHeroAnimInstance* GetHeroAnimInstance() const;
};
