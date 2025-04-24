// SkyRain


#include "GameModes/StarSurvivalGameMode.h"
#include "Engine/AssetManager.h"
#include "Characters/StarEnemyCharacter.h"
#include "StarFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"

#include "StarDebugHelper.h"

void AStarSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	EStarGameDifficulty SavedGameDifficulty;

	// save파일이 있다면 가져와서 난이도 설정.
	if (UStarFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		// CurrentGameDifficulty를 설정 
		// HeroCharacter 는 StarGameMode->GetCurrentGameDifficulty()를 통해서 게임 난이도를 인식.
		// = CurrentGameDifficulty
		CurrentGameDifficulty = SavedGameDifficulty;
	}
}

void AStarSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid datat table in survial game mode blueprint"));

	SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::WaitSpawnNewWave);

	// DataTable의 Row 이름에서 데잉터를 가져옴.
	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	PreLoadNextWaveEnemies();	// 첫 Wave가 시작하기 전에 Enemy Preload
}

void AStarSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// case 1: WaitSpawnNewWave(생성 대기)
	if (CurrentSurvivalGameModeState == EStarSurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaTime;	// 시간 설정

		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0;
			// 새로운 Wave 생성
			SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::SpawningNewWave);
		}
	}

	if (CurrentSurvivalGameModeState == EStarSurvivalGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaTime;

		// case 2 : SpawnEnemiesDelayTime(생성 지연 시간이 지나면)
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			//TODO:Handle spawn new enemies
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

			TimePassedSinceStart = 0;

			SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::InProgress);
		}
	}
	if (CurrentSurvivalGameModeState == EStarSurvivalGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;

		// case 3 : WaveCompletedWaitTime(Wave가 끝나고 대기 시간)
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;

			CurrentWaveCount++;	// Wave 증가

			// Wave가 끝났다면
			if (HasFinishedAllWaves())
			{
				SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::AllWavesDone);
			}
			else // 다음 Wave 가 있다면?
			{
				SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::WaitSpawnNewWave);

				// GameMode State = WaitSpawnNewWave(Spawn대기 상태) 일때 
				// Enemy를 Preload
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AStarSurvivalGameMode::SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState InState)
{
	CurrentSurvivalGameModeState = InState;

	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

bool AStarSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWavesToSpawn;
}

void AStarSurvivalGameMode::PreLoadNextWaveEnemies()
{
	// 모든 Wave를 끝냈을 경우 -> 종료
	if (HasFinishedAllWaves())	return;

	PreLoadedEnemyClassMap.Empty();	// 비워주기 

	// TableRow에서 가져옴 Enemy 배열을 순회 탐색
	for (const FStarEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		// 유효성 검사.
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull())	continue;

		// AsyncLoad
		// UAssetManager::GetStreamableManager().RequestAsyncLoad
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo, this]()
				{
					// Load할 EnemyClass를 가져와서 변수 저장.
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						// EnemyClass의 SoftPtr 과 Class를 Map
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);

						//Debug::Print(LoadedEnemyClass->GetName() + TEXT(" is loaded"));
					}
				}
			)
		);
	}




}

FStarEnemyWaveSpawnerTableRow* AStarSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	// DataTable에서 FStarEnemyWaveSpawnerTableRow에 찾는데 필요한 RowName
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	// 해당 Row에 있는 데이터(FStarEnemyWaveSpawnerTableRow) 를 가져옴.
	FStarEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FStarEnemyWaveSpawnerTableRow>(RowName, FString());

	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"), *RowName.ToString());

	return FoundRow;
}

int32 AStarSurvivalGameMode::TrySpawnWaveEnemies()
{
	// TargetPoint Array가 비어 있다면, GetAllActorsOfClass를 통해 채우기
	// GetAllActorsOfClass-> Level에 있는 모든 액터(Class에 해당하는)를 가져옴.
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// GetCurrentWaveSpawnerTableRow()를 순회 탐색
	for (const FStarEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		// 유효성 검사.
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull())	continue;
		// 해당 Wave에서 Spawn할 수 있는 숫자 설정,
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		// Spawn할 EnemyClass를 가져오고
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; i++)
		{
			// index를 통해 해당 actor의 위치 및 방향(회전 설정)
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			// Random한 위치에 생성
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);

			RandomLocation += FVector(0.f, 0.f, 150.f);

			// Spawn
			AStarEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AStarEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			// Spawn Actor valid? => EnemiesSpawnedThisTime++, TotalSPawn ++
			if (SpawnedEnemy)
			{
				//죽었을 경우, 관련 함수 바인딩.
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);

				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}
			// 소환된 Enemies >= TotalSpawnedEnemiesThisWave 일 경우(해당 Wave에 소환해야할 만큼 한 경우) => 종료
			if (ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	return EnemiesSpawnedThisTime;
}

bool AStarSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	//소환된 Enemies < TotalSpawnedEnemiesThisWave 일 경우( Wave에 필요한 Enemies의 수가 더 있어야함.)
	// => KeepSpawnEnemies
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void AStarSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;

	//Debug::Print(FString::Printf(TEXT("CurrentSpawnedEnemiesCounter:%i, TotalSpawnedEnemiesThisWaveCounter:%i"), CurrentSpawnedEnemiesCounter, TotalSpawnedEnemiesThisWaveCounter));

	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;

		// 완료 처리
		SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::WaveCompleted);
	}
}

void AStarSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AStarEnemyCharacter*>& InEnemiesToRegister)
{
	for (AStarEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;

			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
		}
	}
}
