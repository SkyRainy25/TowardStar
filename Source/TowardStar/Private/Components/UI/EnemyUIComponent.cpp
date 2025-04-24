// SkyRain


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/StarWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(UStarWidgetBase* InWidgetToRegister)
{
	// 위젯 클래스를 배열에 저장
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidgetsIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty())	return;

	for (UStarWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();	// 위젯 제거
		}
	}

	EnemyDrawnWidgets.Empty();
}
