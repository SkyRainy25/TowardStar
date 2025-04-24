// SkyRain


#include "AnimInstances/Hero/StarHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/StarHeroAnimInstance.h"

UStarHeroAnimInstance* UStarHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	return Cast<UStarHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
