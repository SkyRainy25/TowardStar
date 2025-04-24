// SkyRain

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StarTypes/StarEnumTypes.h"
#include "StarFunctionLibrary.generated.h"

class UStarAbilitySystemComponent;
class UPawnCombatComponent;
struct FScalableFloat;
class UStarGameInstance;

/**
 * 
 */
UCLASS()
class TOWARDSTAR_API UStarFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UStarAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	// meta = (DisplayName = "Does Actor Have Tag" 를 통해서
	// 에디터에서 이 함수가 BP_DoesActorHaveTag (X) 아닌 Does Actor Have Tag(O) 표시 되도록 설정하는 매크로
	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EStarConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EStarValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "Star|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	UFUNCTION(BlueprintPure, Category = "Star|FunctionLibrary", meta = (CompactNodeTitle = "Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.f);

	UFUNCTION(BlueprintPure, Category = "Star|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	UFUNCTION(BlueprintPure, Category = "Star|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo", ExpandEnumAsExecs = "CountDownInput|CountDownOutput", TotalTime = "1.0", UpdateInterval = "0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime, EStarCountDownActionInput CountDownInput,
		EStarCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintPure, Category = "Star|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static UStarGameInstance* GetStarGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static void ToggleInputMode(const UObject* WorldContextObject, EStarInputMode InInputMode);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static void SaveCurrentGameDifficulty(EStarGameDifficulty InDifficultyToSave);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static bool TryLoadSavedGameDifficulty(EStarGameDifficulty& OutSavedDifficulty);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static TArray<FVector> GetSpawnLocationAroundTargetPoints(const FVector& InStart, const FVector& InEnd, const int32& InSpawnNum, const float& InSpawnRadius, const float& InSpawnAngle);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static FVector GetRotateLocationFromOrigin(const FVector& InOrigin, const FVector& InForward, const float& InRotation);

	UFUNCTION(BlueprintCallable, Category = "Star|FunctionLibrary")
	static TArray<FVector> GetRandomLocatioFromOrigin(const FVector& InOrigin, const float& InRadius, const float& InHeight, const float& InAngle, const int32& InSpawnNum, const float& InRange);
};
