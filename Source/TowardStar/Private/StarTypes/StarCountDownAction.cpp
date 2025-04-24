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
		
	// �帥 �ð� ���
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		// �帥 �ð��� �����ð��� ����. Current Time = PreviousTime + deltaTime
		ElapsedTimeSinceStart += ElapsedInterval > 0.f ? UpdateInterval : Response.ElapsedTime();

		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;	// ���� �ð� ���
		
		CountDownOutput = EStarCountDownActionOutput::Updated;

		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);	// Blueprint�� Link

		ElapsedInterval = 0.f;	// �ʱ�ȭ
	}
}

void FStarCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
