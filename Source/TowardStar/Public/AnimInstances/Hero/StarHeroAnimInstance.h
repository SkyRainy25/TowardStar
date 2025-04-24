// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/StarCharacterAnimInstance.h"
#include "StarHeroAnimInstance.generated.h"

class AStarHeroCharacter;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarHeroAnimInstance : public UStarCharacterAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);	

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Refrences")
	AStarHeroCharacter* OwningHeroChracter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;
	
	// 애니메이션 전환을 위한 임계시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxtStateThreshold = 5.f;

	// IdleAnimation으로 전환되고 난 후의 시간을 추적하기 위함.
	float IdleElpasedTime;
};
