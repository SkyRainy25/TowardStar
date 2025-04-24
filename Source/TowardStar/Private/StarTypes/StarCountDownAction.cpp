// SkyRain


#include "StarTypes/StarCountDownAction.h"

void FStarCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{
		CountDownOutput = EStarCountDownActionOutput::Cancelled;

		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		
		return;
	}

	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		CountDownOutput = EStarCountDownActionOutput::Completed;

		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);

		return;
	}
		
	// 흐른 시간 계산
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		// 흐른 시간을 지난시간에 누적. Current Time = PreviousTime + deltaTime
		ElapsedTimeSinceStart += ElapsedInterval > 0.f ? UpdateInterval : Response.ElapsedTime();

		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;	// 남은 시간 계산
		
		CountDownOutput = EStarCountDownActionOutput::Updated;

		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);	// Blueprint에 Link

		ElapsedInterval = 0.f;	// 초기화
	}
}

void FStarCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
