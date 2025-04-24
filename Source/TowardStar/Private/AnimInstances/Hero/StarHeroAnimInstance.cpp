// SkyRain


#include "AnimInstances/Hero/StarHeroAnimInstance.h"
#include "Characters/StarHeroCharacter.h"

void UStarHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningCharacter = Cast<AStarHeroCharacter>(OwningCharacter);
	}
}

void UStarHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (bHasAcceleration)	// 움직이고 있는 상태
	{
		IdleElpasedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		// Idle 상태로 진입후 지난 시간을 DeltaSeconds를 통해 누적계산.
		IdleElpasedTime += DeltaSeconds;
		// 임계시간보다 클경우 boolean 전환
		bShouldEnterRelaxState = IdleElpasedTime >= EnterRelaxtStateThreshold;
	}
}
