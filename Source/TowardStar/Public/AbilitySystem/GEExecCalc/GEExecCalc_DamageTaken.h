// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExecCalc_DamageTaken.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UGEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGEExecCalc_DamageTaken();

	// �θ� Ŭ������ Execute �Լ��� BlueprintNativeEvent �̱� ������
	// �Լ� �̸� + _Implementaion�� �ٿ��ش�.
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
