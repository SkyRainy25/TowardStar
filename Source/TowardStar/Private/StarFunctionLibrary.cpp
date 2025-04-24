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

	// 추가하려는 태그가 ASC에 없다면? => 추가
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UStarFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UStarAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	// 제거하려는 태그가 ASC에 있다면? => 제거
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
	// 확인할 태그를 가지고 있으면 Yes를 없으면 No 반환.
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EStarConfirmType::Yes : EStarConfirmType::No;
}

UPawnCombatComponent* UStarFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	// Hero or Enemy에 맞는 CombatComponent를 가져옴
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
		// GernericTeamId가 같은지 확인 후 결괏값 반환.
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

	// a X b (CrossProduct) : Unreal에서는 왼손 법칙을 따름.
	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	// CrossResult = 음수? => 아래방향을 가리킴 
	// Angle x -1 을 해줘 각도 보정.
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}
	
	// 플레이어의 ForwardVector를 y축으로 4분면을 그려서 
	// 적이 플레이어를 피격한 위치 판단.
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

	// DotResult < 0 (음수) 서로 마주보고 있음.
	// 0에 근사한 값이 valid하는 것을 방지하기 위해 -0.1 사용
	return DotResult < -0.1f;
}

bool UStarFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UStarAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	UStarAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	// InputParam으로 넘겨받은 GESpecHandle(Projectile에서 건네받음)을 ApplayGESpecToTarget을 통해 데미지 적용
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
	
	// 성공적으로 적용이 됐는지를 반환
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

	// CountDown 클래스
	FStarCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FStarCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	// 시작했는데 Action이 없다면?
	if (CountDownInput == EStarCountDownActionInput::Start)
	{
		if (!FoundAction)	// 없다면 추가
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,	// Action object
				LatentInfo.UUID,			// Unique ID
				new FStarCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo)	// 동적 할당으로 클래스 생성.
			);
		}
	}

	// Countdown Action 을 취소
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
	// save 파일 생성( save할 정보를 설정 및 저장)
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UStarSaveGame::StaticClass());

	if (UStarSaveGame* StarSaveGameObject = Cast<UStarSaveGame>(SaveGameObject))
	{
		// 난이도 저장
		StarSaveGameObject->SavedCurrentGameDifficulty = InDifficultyToSave;

		// 세이브	(Syncronous Saving) 
		const bool bWasSaved = UGameplayStatics::SaveGameToSlot(StarSaveGameObject, StarGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		Debug::Print(bWasSaved ? TEXT("Difficulty Saved") : TEXT("Difficulty NOT Saved"));
	}
}

bool UStarFunctionLibrary::TryLoadSavedGameDifficulty(EStarGameDifficulty& OutSavedDifficulty)
{
	// 해당 슬롯이 존재한다면?
	if (UGameplayStatics::DoesSaveGameExist(StarGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0.f))
	{
		// 가져오고
		USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(StarGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		if (UStarSaveGame* StarSaveGameObject = Cast<UStarSaveGame>(SaveGameObject))
		{
			// 난이도 정보 가져옴
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

	// 시작 = 끝점 => 자신 주위로 Spawn할 경우
	if (ForwardVector == FVector(0,0,0))
	{
		// 바라보는 방향이 기준
		ForwardVector = InStart.ForwardVector;
	}
	ForwardVector.Normalize();

	// 왼쪽 끝각 ~ 오른쪽 끝각으로 범위 설정
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
	// Z축(UpVector)를 기준으로 InRotation(Theta)만큼씩 증가. 
	return InOrigin + (InForward).RotateAngleAxis(InRotation, InOrigin.UpVector);
}

TArray<FVector> UStarFunctionLibrary::GetRandomLocatioFromOrigin(const FVector& InOrigin, const float& InRadius, const float& InHeight, const float& InAngle, const int32& InSpawnNum, const float& InRange)
{
	TArray<FVector> OutSpawnLocations;

	// 시작점의 ForwardVector를 2D로 Normalize
	FVector ForwardVector = InOrigin.ForwardVector.GetSafeNormal2D();

	float StartAngle = -(InAngle / 2);
	float EndAngle = InAngle / 2;
	float DeltaAngle = InAngle / (InSpawnNum - 1); // 간격

	float Range = InRange;	// 

	for (float i = StartAngle; i <= EndAngle; i += DeltaAngle)
	{
		// 해당 각도의 랜덤한 위치에 생성 포인트 걸정.
		float RandLocation = FMath::RandRange(InRadius - Range, InRadius);
		FVector SpawnLocation = InOrigin + (ForwardVector * RandLocation).RotateAngleAxis(i, InOrigin.UpVector) + FVector(0.f, 0.f, InHeight);

		OutSpawnLocations.Add(SpawnLocation);	// OutParam 배열에 추가.
	}

	return OutSpawnLocations;
}
