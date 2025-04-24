// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "StarTypes/StarEnumTypes.h"

/**
 * 
 */
class FStarCountDownAction : public FPendingLatentAction
{
public:
    FStarCountDownAction(float InTotalCountDownTime, float InUpdateInterval, float& InOutRemainingTime, EStarCountDownActionOutput& InCountDownOutput, const FLatentActionInfo& LatentInfo)
        : bNeedToCancel(false)
        , TotalCountDownTime(InTotalCountDownTime)
        , UpdateInterval(InUpdateInterval)
        , OutRemainingTime(InOutRemainingTime)
        , CountDownOutput(InCountDownOutput)
        , ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
        , ElapsedInterval(0.f)
        , ElapsedTimeSinceStart(0.f)
    {
    }
    virtual void UpdateOperation(FLatentResponse& Response) override;

    void CancelAction();

private:
    bool bNeedToCancel;         // ���
    float TotalCountDownTime;   // ��ü ��Ÿ��
    float UpdateInterval;       // ������Ʈ �ֱ�
    float& OutRemainingTime;    // �����ð�(OutParam)
    EStarCountDownActionOutput& CountDownOutput;    //
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;
    float ElapsedInterval;
    float ElapsedTimeSinceStart;
};
