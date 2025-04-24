// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/StarBaseAnimInstance.h"
#include "StarCharacterAnimInstance.generated.h"

class AStarBaseCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarCharacterAnimInstance : public UStarBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	// UPROPERTY() : �����Ͱ� nullptr�� ��, Garbage ������ ���� ��ũ��. ȿ������ ������ ������ ���� ��ũ��
	UPROPERTY()
	AStarBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;
};
