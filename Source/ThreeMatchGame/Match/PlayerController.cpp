#include "PlayerController.h"

#include "Kismet/GameplayStatics.h"


AMPlayerController::AMPlayerController(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	// We want the mouse cursor to show immediately on startup, without having to click in the window.
	bShowMouseCursor = true;

	// We need click/touch events to interact with our tiles.
	bEnableTouchEvents = bEnableClickEvents = true;
	bEnableTouchOverEvents = bEnableMouseOverEvents = true;

	ScoreChangeRate = 375.0f;
}

void AMPlayerController::AddScore(int32 Points, bool bForceImmediateUpdate)
{
	Score += Points;
	if (bForceImmediateUpdate)
	{
		DisplayedScore = (float)Score;
	}
	else
	{
		GetWorldTimerManager().SetTimer(TickScoreDisplayHandle, this, &AMPlayerController::TickScoreDisplay, 0.001f, true);
	}
}

int32 AMPlayerController::GetScore()
{
	return Score;
}

int32 AMPlayerController::GetDisplayedScore()
{
	return (int32)DisplayedScore;
}

int32 AMPlayerController::CalculateBombPower_Implementation()
{
	return 0;
}

void AMPlayerController::TickScoreDisplay()
{
	// This assumes score only goes up, or instantly drops when it is decreased.
	DisplayedScore += UGameplayStatics::GetWorldDeltaSeconds(this) * ScoreChangeRate;
	if (DisplayedScore >= (float)Score)
	{
		DisplayedScore = Score;
		GetWorldTimerManager().ClearTimer(TickScoreDisplayHandle);
	}
}
