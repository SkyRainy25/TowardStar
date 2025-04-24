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

	if (bHasAcceleration)	// �����̰� �ִ� ����
	{
		IdleElpasedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		// Idle ���·� ������ ���� �ð��� DeltaSeconds�� ���� �������.
		IdleElpasedTime += DeltaSeconds;
		// �Ӱ�ð����� Ŭ��� boolean ��ȯ
		bShouldEnterRelaxState = IdleElpasedTime >= EnterRelaxtStateThreshold;
	}
}
