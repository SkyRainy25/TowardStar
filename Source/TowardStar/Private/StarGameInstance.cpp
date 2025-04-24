// SkyRain


#include "StarGameInstance.h"
#include "MoviePlayer.h"

void UStarGameInstance::Init()
{
    Super::Init();

    // 델리게이트와 바인딩
    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);
}

// LoadMap을 열려고 할 때
void UStarGameInstance::OnPreLoadMap(const FString& MapName)
{
    FLoadingScreenAttributes LoadingScreenAttributes;
    LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
    LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
    // Loading을 하는 동안 띄울 Widget, Screen, Texture, Video 등과 관련된 설정
    /** The widget to be displayed on top of the movie or simply standalone if there is no movie. */
    LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
    
    // LoadingScreent을 설정.(구조체를 통해 설정된 데이터를 기반하여)
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

// LoadMap이 열리기 직전 
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
