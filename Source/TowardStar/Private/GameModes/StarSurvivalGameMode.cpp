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

	// save������ �ִٸ� �����ͼ� ���̵� ����.
	if (UStarFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		// CurrentGameDifficulty�� ���� 
		// HeroCharacter �� StarGameMode->GetCurrentGameDifficulty()�� ���ؼ� ���� ���̵��� �ν�.
		// = CurrentGameDifficulty
		CurrentGameDifficulty = SavedGameDifficulty;
	}
}

void AStarSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid datat table in survial game mode blueprint"));

	SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::WaitSpawnNewWave);

	// DataTable�� Row �̸����� �����͸� ������.
	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	PreLoadNextWaveEnemies();	// ù Wave�� �����ϱ� ���� Enemy Preload
}

void AStarSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// case 1: WaitSpawnNewWave(���� ���)
	if (CurrentSurvivalGameModeState == EStarSurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaTime;	// �ð� ����

		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0;
			// ���ο� Wave ����
			SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::SpawningNewWave);
		}
	}

	if (CurrentSurvivalGameModeState == EStarSurvivalGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaTime;

		// case 2 : SpawnEnemiesDelayTime(���� ���� �ð��� ������)
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

		// case 3 : WaveCompletedWaitTime(Wave�� ������ ��� �ð�)
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;

			CurrentWaveCount++;	// Wave ����

			// Wave�� �����ٸ�
			if (HasFinishedAllWaves())
			{
				SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::AllWavesDone);
			}
			else // ���� Wave �� �ִٸ�?
			{
				SetCurrentSurvivalGameModeState(EStarSurvivalGameModeState::WaitSpawnNewWave);

				// GameMode State = WaitSpawnNewWave(Spawn��� ����) �϶� 
				// Enemy�� Preload
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
	// ��� Wave�� ������ ��� -> ����
	if (HasFinishedAllWaves())	return;

	PreLoadedEnemyClassMap.Empty();	// ����ֱ� 

	// TableRow���� ������ Enemy �迭�� ��ȸ Ž��
	for (const FStarEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		// ��ȿ�� �˻�.
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull())	continue;

		// AsyncLoad
		// UAssetManager::GetStreamableManager().RequestAsyncLoad
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo, this]()
				{
					// Load�� EnemyClass�� �����ͼ� ���� ����.
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						// EnemyClass�� SoftPtr �� Class�� Map
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
	// DataTable���� FStarEnemyWaveSpawnerTableRow�� ã�µ� �ʿ��� RowName
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	// �ش� Row�� �ִ� ������(FStarEnemyWaveSpawnerTableRow) �� ������.
	FStarEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FStarEnemyWaveSpawnerTableRow>(RowName, FString());

	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"), *RowName.ToString());

	return FoundRow;
}

int32 AStarSurvivalGameMode::TrySpawnWaveEnemies()
{
	// TargetPoint Array�� ��� �ִٸ�, GetAllActorsOfClass�� ���� ä���
	// GetAllActorsOfClass-> Level�� �ִ� ��� ����(Class�� �ش��ϴ�)�� ������.
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// GetCurrentWaveSpawnerTableRow()�� ��ȸ Ž��
	for (const FStarEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		// ��ȿ�� �˻�.
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull())	continue;
		// �ش� Wave���� Spawn�� �� �ִ� ���� ����,
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		// Spawn�� EnemyClass�� ��������
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; i++)
		{
			// index�� ���� �ش� actor�� ��ġ �� ����(ȸ�� ����)
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			// Random�� ��ġ�� ����
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);

			RandomLocation += FVector(0.f, 0.f, 150.f);

			// Spawn
			AStarEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AStarEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			// Spawn Actor valid? => EnemiesSpawnedThisTime++, TotalSPawn ++
			if (SpawnedEnemy)
			{
				//�׾��� ���, ���� �Լ� ���ε�.
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);

				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}
			// ��ȯ�� Enemies >= TotalSpawnedEnemiesThisWave �� ���(�ش� Wave�� ��ȯ�ؾ��� ��ŭ �� ���) => ����
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
	//��ȯ�� Enemies < TotalSpawnedEnemiesThisWave �� ���( Wave�� �ʿ��� Enemies�� ���� �� �־����.)
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

		// �Ϸ� ó��
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
