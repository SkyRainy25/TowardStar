// SkyRain


#include "Controllers/StarPlayerController.h"

AStarPlayerController::AStarPlayerController()
{
	// PC = 0 / Enemy = 1�� ���� ����.
	HeroTeamID = FGenericTeamId(0);
}

FGenericTeamId AStarPlayerController::GetGenericTeamId() const
{
	return HeroTeamID;
}
