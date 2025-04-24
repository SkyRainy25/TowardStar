// SkyRain


#include "Widgets/StarWidgetBase.h"
#include "Interfaces/PawnUIInterface.h"

void UStarWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			// PawnUIInterface를 통해서 HeroUIComponent와 바인딩?
			// StarWidgetBase를 기반으로 하는 클래스는 자동으로 실행(바인딩됨)
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}

void UStarWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();

		checkf(EnemyUIComponent, TEXT("Failed to extrac an EnemyUIComponent from %s"), *OwningEnemyActor->GetActorNameOrLabel());

		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}
