// SkyRain


#include "AnimInstances/StarBaseAnimInstance.h"
#include "StarFunctionLibrary.h"


bool UStarBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		// OwningPawn이 TagToCheck를 가지고 있는지 확인하여 결괏값 반환.
		return UStarFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
	}

	return false;
}
