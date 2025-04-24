// SkyRain


#include "StarGameInstance.h"
#include "MoviePlayer.h"

void UStarGameInstance::Init()
{
    Super::Init();

    // ��������Ʈ�� ���ε�
    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);
}

// LoadMap�� ������ �� ��
void UStarGameInstance::OnPreLoadMap(const FString& MapName)
{
    FLoadingScreenAttributes LoadingScreenAttributes;
    LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
    LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
    // Loading�� �ϴ� ���� ��� Widget, Screen, Texture, Video ��� ���õ� ����
    /** The widget to be displayed on top of the movie or simply standalone if there is no movie. */
    LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
    
    // LoadingScreent�� ����.(����ü�� ���� ������ �����͸� ����Ͽ�)
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

// LoadMap�� ������ ���� 
void UStarGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
    // Close Loading Screen
    GetMoviePlayer()->StopMovie();
}

TSoftObjectPtr<UWorld> UStarGameInstance::GetGameLevelByTag(FGameplayTag InTag) const
{
	for (const FStarGameLevelSet& GameLevelSet : GameLevelSets)
    {
        if (!GameLevelSet.IsValid()) continue;

        if (GameLevelSet.LevelTag == InTag)
        {
            return GameLevelSet.Level;
        }
    }

    return TSoftObjectPtr<UWorld>();
}
