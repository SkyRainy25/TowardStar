// SkyRain


#include "AI/BTTask_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate to Face Target Actor");	// ����̸�
	AnglePrecision = 10.f;
	RotationInterpSpeed = 5.f;

	bNotifyTick = true;		// Tick Function�� �����ϱ� ����.
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;	
	//�ν��Ͻ��� ������ �ʴ´ٴ� ���� �⺻������ Data�� �������� �ʴ´�. => ���ŵǸ� ������ ������ ��� �Ұ�.
	// ����ü�� �̿��ؼ� ������ �����͸� ����

	INIT_TASK_NODE_NOTIFY_FLAGS();

	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());

}

void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	// ����ü�� ũ�⸦ ��ȯ�ϴ� �Լ�.
	return sizeof(FRotateToFaceTargetTaskMemory);
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision: %s is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	// ����ü ����.
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);

	// Target / OwningPawn ����.
	Memory->TargetActor = TargetActor;
	Memory->OwningPawn = OwningPawn;
		 
	//��ȿ���� ���� ��� Fail
	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}
	// ���ǿ� �����ϸ� ����.
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();	// Memory Reset
		return EBTNodeResult::Succeeded;
	}

	// �ƴ� ��� ������.
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	// �޸� ��ȿ�� �˻� => FinishLatentTask(Fail)
	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	// AnglePrecision�� ���޽� => Reset + FinishLatentTask(Succeeded)
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else    // AnglePrecision ���� x => ĳ���� ȸ��.
	{
		// OwningPawn - Target ������ ȸ������ ���ϰ�
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		// OwningPawn�� ȸ�������� LookAtRot���� �����ӵ��� ȸ��.
		const FRotator TargetRot = FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);

		// TargetRot = ��ǥȸ����(LookAtRot)������ ���� => �׸�ŭ SetActorRotation�� ���� ������.
		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

bool UBTTask_RotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector OwnerForward = QueryPawn->GetActorForwardVector();
	// Normalize DistanceToTarget vector
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();

	// DotProduct�� �ᱣ���� ������ ġȯ. �������� ���� ����(theta)�� ����.
	const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);

	// AnglePrecision �� �����ߴ���
	return AngleDiff <= AnglePrecision;
}
