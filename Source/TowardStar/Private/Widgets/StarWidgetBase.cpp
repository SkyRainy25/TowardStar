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
			// PawnUIInterface�� ���ؼ� HeroUIComponent�� ���ε�?
			// StarWidgetBase�� ������� �ϴ� Ŭ������ �ڵ����� ����(���ε���)
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
