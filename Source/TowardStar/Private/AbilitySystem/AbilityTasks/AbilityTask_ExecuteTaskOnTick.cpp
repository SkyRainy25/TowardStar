// SkyRain


#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteTaskOnTick.h"

UAbilityTask_ExecuteTaskOnTick::UAbilityTask_ExecuteTaskOnTick()
{
	// TickTask를 호출하기 위한 설정.
	bTickingTask = true;
}

UAbilityTask_ExecuteTaskOnTick* UAbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	UAbilityTask_ExecuteTaskOnTick* Node = NewAbilityTask< UAbilityTask_ExecuteTaskOnTick>(OwningAbility);

	return Node;
}

void UAbilityTask_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// AbilityGraph에 Broadcast를 하기 전에 호출해야하는 함수.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAbilityTaskTick.Broadcast(DeltaTime);	// 
	}
	else
	{
		EndTask();
	}
}
