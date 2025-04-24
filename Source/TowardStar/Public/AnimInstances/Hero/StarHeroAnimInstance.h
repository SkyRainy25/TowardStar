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
	
	// �ִϸ��̼� ��ȯ�� ���� �Ӱ�ð�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxtStateThreshold = 5.f;

	// IdleAnimation���� ��ȯ�ǰ� �� ���� �ð��� �����ϱ� ����.
	float IdleElpasedTime;
};
