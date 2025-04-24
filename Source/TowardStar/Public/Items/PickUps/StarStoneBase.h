// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUps/StarPickUpBase.h"
#include "StarStoneBase.generated.h"

class UStarAbilitySystemComponent;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class TOWARDSTAR_API AStarStoneBase : public AStarPickUpBase
{
	GENERATED_BODY()

public:
	void Consume(UStarAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel);

protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Stone Consumed"))
	void BP_OnStoneConsumed();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
};
