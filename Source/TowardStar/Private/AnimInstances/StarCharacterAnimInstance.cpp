// SkyRain


#include "AnimInstances/StarCharacterAnimInstance.h"
#include "Characters/StarBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

// UE������ ȣ���� ��.
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
	
	// UKismetAnimationLibrary�� CalculateDirection�� �̿��Ͽ� LocomotionDirection ���
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
