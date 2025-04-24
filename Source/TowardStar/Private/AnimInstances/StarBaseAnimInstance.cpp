// SkyRain


#include "AnimInstances/StarBaseAnimInstance.h"
#include "StarFunctionLibrary.h"


bool UStarBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		// OwningPawn�� TagToCheck�� ������ �ִ��� Ȯ���Ͽ� �ᱣ�� ��ȯ.
		return UStarFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
	}

	return false;
}
