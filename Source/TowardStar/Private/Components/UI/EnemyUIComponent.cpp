// SkyRain


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/StarWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(UStarWidgetBase* InWidgetToRegister)
{
	// ���� Ŭ������ �迭�� ����
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidgetsIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty())	return;

	for (UStarWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();	// ���� ����
		}
	}

	EnemyDrawnWidgets.Empty();
}
