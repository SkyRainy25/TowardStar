// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarPickUpBase.generated.h"

class USphereComponent;

UCLASS()
class TOWARDSTAR_API AStarPickUpBase : public AActor
{
	GENERATED_BODY()

public:
	AStarPickUpBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pick UP Interaction")
	USphereComponent* PickUpCollisionSphere;

	UFUNCTION()
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
