// SkyRain


#include "StarFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/StarAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"

#include "StarGameInstance.h"
#include "StarTypes/StarCountDownAction.h"
#include "StarGameplayTags.h"
#include "StarDebugHelper.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGame/StarSaveGame.h"

UStarAbilitySystemComponent* UStarFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UStarAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UStarFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UStarAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	// �߰��Ϸ��� �±װ� ASC�� ���ٸ�? => �߰�
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UStarFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UStarAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	// �����Ϸ��� �±װ� ASC�� �ִٸ�? => ����
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UStarFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UStarAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UStarFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EStarConfirmType& OutConfirmType)
{
	// Ȯ���� �±׸� ������ ������ Yes�� ������ No ��ȯ.
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EStarConfirmType::Yes : EStarConfirmType::No;
}

UPawnCombatComponent* UStarFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	// Hero or Enemy�� �´� CombatComponent�� ������
	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

UPawnCombatComponent* UStarFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EStarValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent? EStarValidType::Valid : EStarValidType::Invalid;

	return CombatComponent;
}

bool UStarFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);

	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		// GernericTeamId�� ������ Ȯ�� �� �ᱣ�� ��ȯ.
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}

	return false;
}

float UStarFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UStarFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	// a X b (CrossProduct) : Unreal������ �޼� ��Ģ�� ����.
	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	// CrossResult = ����? => �Ʒ������� ����Ŵ 
	// Angle x -1 �� ���� ���� ����.
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}
	
	// �÷��̾��� ForwardVector�� y������ 4�и��� �׷��� 
	// ���� �÷��̾ �ǰ��� ��ġ �Ǵ�.
	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
	{
		return StarGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		return StarGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.f || OutAngleDifference>135.f)
	{
		return StarGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return StarGameplayTags::Shared_Status_HitReact_Right;
	}

	return StarGameplayTags::Shared_Status_HitReact_Front;
}

bool UStarFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	const FString DebugString = FString::Printf(TEXT("Dot Result: %f %s"), DotResult, DotResult < -0.1f ? TEXT("Valid Block") : TEXT("InvalidBlock"));

	Debug::Print(DebugString, DotResult < -0.1f ? FColor::Green : FColor::Red);

	// DotResult < 0 (����) ���� ���ֺ��� ����.
	// 0�� �ٻ��� ���� valid�ϴ� ���� �����ϱ� ���� -0.1 ���
	return DotResult < -0.1f;
}

bool UStarFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UStarAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	UStarAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	// InputParam���� �Ѱܹ��� GESpecHandle(Projectile���� �ǳ׹���)�� ApplayGESpecToTarget�� ���� ������ ����
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
	
	// ���������� ������ �ƴ����� ��ȯ
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UStarFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime, EStarCountDownActionInput CountDownInput, EStarCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;

	if (GEngine)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return;
	}

	// Store Reference Variable
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	// CountDown Ŭ����
	FStarCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FStarCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	// �����ߴµ� Action�� ���ٸ�?
	if (CountDownInput == EStarCountDownActionInput::Start)
	{
		if (!FoundAction)	// ���ٸ� �߰�
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,	// Action object
				LatentInfo.UUID,			// Unique ID
				new FStarCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo)	// ���� �Ҵ����� Ŭ���� ����.
			);
		}
	}

	// Countdown Action �� ���
	if (CountDownInput == EStarCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}
}

UStarGameInstance* UStarFunctionLibrary::GetStarGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetGameInstance<UStarGameInstance>();
		}
	}

	return nullptr;
}

void UStarFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EStarInputMode InInputMode)
{
	APlayerController* PlayerController = nullptr;

	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}
	}

	if (!PlayerController)	return;

	FInputModeGameOnly GameOnlyMode;
	FInputModeUIOnly UIOnlyMode;

	switch (InInputMode)
	{
	case EStarInputMode::GameOnly:

		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor = false;

		break;

	case EStarInputMode::UIOnly:

		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor = true;

		break;

	default:
		break;
	}
}

void UStarFunctionLibrary::SaveCurrentGameDifficulty(EStarGameDifficulty InDifficultyToSave)
{
	// save ���� ����( save�� ������ ���� �� ����)
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UStarSaveGame::StaticClass());

	if (UStarSaveGame* StarSaveGameObject = Cast<UStarSaveGame>(SaveGameObject))
	{
		// ���̵� ����
		StarSaveGameObject->SavedCurrentGameDifficulty = InDifficultyToSave;

		// ���̺�	(Syncronous Saving) 
		const bool bWasSaved = UGameplayStatics::SaveGameToSlot(StarSaveGameObject, StarGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		Debug::Print(bWasSaved ? TEXT("Difficulty Saved") : TEXT("Difficulty NOT Saved"));
	}
}

bool UStarFunctionLibrary::TryLoadSavedGameDifficulty(EStarGameDifficulty& OutSavedDifficulty)
{
	// �ش� ������ �����Ѵٸ�?
	if (UGameplayStatics::DoesSaveGameExist(StarGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0.f))
	{
		// ��������
		USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(StarGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		if (UStarSaveGame* StarSaveGameObject = Cast<UStarSaveGame>(SaveGameObject))
		{
			// ���̵� ���� ������
			OutSavedDifficulty = StarSaveGameObject->SavedCurrentGameDifficulty;

			Debug::Print(TEXT("Loading Successful"), FColor::Green);

			return true;
		}
	}
	return false;
}

TArray<FVector> UStarFunctionLibrary::GetSpawnLocationAroundTargetPoints(const FVector& InStart, const FVector& InEnd, const int32& InSpawnNum, 
	const float& InSpawnRadius, const float& InSpawnAngle)
{
	TArray<FVector> OutSpawnLocations;
	FVector ForwardVector = (InEnd - InStart);

	// ���� = ���� => �ڽ� ������ Spawn�� ���
	if (ForwardVector == FVector(0,0,0))
	{
		// �ٶ󺸴� ������ ����
		ForwardVector = InStart.ForwardVector;
	}
	ForwardVector.Normalize();

	// ���� ���� ~ ������ �������� ���� ����
	int32 StartAngle = -InSpawnAngle * (InSpawnNum - 1) / 2;
	int32 EndAngle = InSpawnAngle * (InSpawnNum - 1) / 2;
	for (int32 i = StartAngle; i <= EndAngle; i += InSpawnAngle)
	{
		//FVector SpawnLocation = GetRotateLocationFromOrigin(InEnd, ForwardVector * InSpawnRadius, i);
		FVector SpawnLocation = InEnd + (ForwardVector*InSpawnRadius).RotateAngleAxis(i, InEnd.UpVector);
		OutSpawnLocations.AddUnique(SpawnLocation);
	}

	return OutSpawnLocations;
}

FVector UStarFunctionLibrary::GetRotateLocationFromOrigin(const FVector& InOrigin, const FVector& InForward, const float& InRotation)
{
	// Z��(UpVector)�� �������� InRotation(Theta)��ŭ�� ����. 
	return InOrigin + (InForward).RotateAngleAxis(InRotation, InOrigin.UpVector);
}

TArray<FVector> UStarFunctionLibrary::GetRandomLocatioFromOrigin(const FVector& InOrigin, const float& InRadius, const float& InHeight, const float& InAngle, const int32& InSpawnNum, const float& InRange)
{
	TArray<FVector> OutSpawnLocations;

	// �������� ForwardVector�� 2D�� Normalize
	FVector ForwardVector = InOrigin.ForwardVector.GetSafeNormal2D();

	float StartAngle = -(InAngle / 2);
	float EndAngle = InAngle / 2;
	float DeltaAngle = InAngle / (InSpawnNum - 1); // ����

	float Range = InRange;	// 

	for (float i = StartAngle; i <= EndAngle; i += DeltaAngle)
	{
		// �ش� ������ ������ ��ġ�� ���� ����Ʈ ����.
		float RandLocation = FMath::RandRange(InRadius - Range, InRadius);
		FVector SpawnLocation = InOrigin + (ForwardVector * RandLocation).RotateAngleAxis(i, InOrigin.UpVector) + FVector(0.f, 0.f, InHeight);

		OutSpawnLocations.Add(SpawnLocation);	// OutParam �迭�� �߰�.
	}

	return OutSpawnLocations;
}
