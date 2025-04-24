// SkyRain


#include "AbilitySystem/GEExecCalc/GEExecCalc_DamageTaken.h"
#include "AbilitySystem/StarAttributeSet.h"
#include "StarGameplayTags.h"

#include "StarDebugHelper.h"

struct FStarDamageCapture
{
	// Slow way�� ����� ������ ��ũ��
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FStarDamageCapture()	// ����ü ������
	{
		// Caputure�� Attribute�� ����
		// (Scope , PropertyName, Sorce or Target, Wheter to SnapShot) => (AttributeName)Def�� ���ٺ��� ����
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStarAttributeSet, Attack, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStarAttributeSet, Defense, Target, false)	// �������� ����� ���� Target�� defense�� ����ϹǷ�.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStarAttributeSet, DamageTaken, Target, false)
	}
};

// Get Func(static)
static const FStarDamageCapture& GetStarDamageCapture()
{
	// ����(static) ����ü ���� + return
	static FStarDamageCapture StarDamageCapture;
	return StarDamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	/*Slow way of doing capture*/
	//FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
	//	UWarriorAttributeSet::StaticClass(),
	//	GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet,AttackPower)
	//);

	//FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
	//	AttackPowerProperty,
	//	EGameplayEffectAttributeCaptureSource::Source,
	//	false
	//);

	//RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);

	// ĸ���� Attribute(Def)�� �迭�� �߰�.
	// ����ü���� AttributeDef(Typd : FGameplayEffectAttributeCaptureDefinition)�� ������. 
	// add()�� �Ű� ���� : FGameplayEffectAttributeCaptureDefinition
	RelevantAttributesToCapture.Add(GetStarDamageCapture().AttackDef);	
	RelevantAttributesToCapture.Add(GetStarDamageCapture().DefenseDef);
	RelevantAttributesToCapture.Add(GetStarDamageCapture().DamageTakenDef);

}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	/*EffectSpec.GetContext().GetSourceObject();
	EffectSpec.GetContext().GetAbility();
	EffectSpec.GetContext().GetInstigator();
	EffectSpec.GetContext().GetEffectCauser();*/

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttack = 0;
	// Cature�� AttackDef ���� Attack ������ ����.
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetStarDamageCapture().AttackDef, EvaluateParameters, SourceAttack);
	
	//Debug::Print(TEXT("SourceAttackPower"), SourceAttack);

	float BaseDamage = 0;
	int32 UsedLightAttckComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;

	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(StarGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
			//Debug::Print(TEXT("BaseDamage"), BaseDamage);
		}
		if (TagMagnitude.Key.MatchesTagExact(StarGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttckComboCount = TagMagnitude.Value;
			//Debug::Print(TEXT("UsedLightAttckComboCount"), UsedLightAttckComboCount);
		}		
		if (TagMagnitude.Key.MatchesTagExact(StarGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount = TagMagnitude.Value;
			//Debug::Print(TEXT("UsedLightAttckComboCount"), UsedHeavyAttackComboCount);
		}
	}
	float TargetDefense = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetStarDamageCapture().DefenseDef, EvaluateParameters, TargetDefense);
	
	//Debug::Print(TEXT("TargetDefensePower"), TargetDefense);

	if (UsedLightAttckComboCount != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightAttckComboCount - 1) * 0.05 + 1.f;
		BaseDamage *= DamageIncreasePercentLight;
		//Debug::Print(TEXT("ScaledBaseDamageLight"), BaseDamage);
	}
	if (UsedHeavyAttackComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = (UsedHeavyAttackComboCount - 1) * 0.25 + 1.f;
		BaseDamage *= DamageIncreasePercentHeavy;
		//Debug::Print(TEXT("ScaledBaseDamageHeavy"), BaseDamage);
	}

	const float FinalDamage = BaseDamage + SourceAttack - TargetDefense;

	//Debug::Print(TEXT("FinalDamageDone"), FinalDamage);

	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetStarDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamage
				)
		);
	}
}
