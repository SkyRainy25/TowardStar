// SkyRain


#include "AnimInstances/StarCharacterAnimInstance.h"
#include "Characters/StarBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

// UE에서도 호출이 됨.
void UStarCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<AStarBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UStarCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)	return;

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
	
	// UKismetAnimationLibrary의 CalculateDirection를 이용하여 LocomotionDirection 계산
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
