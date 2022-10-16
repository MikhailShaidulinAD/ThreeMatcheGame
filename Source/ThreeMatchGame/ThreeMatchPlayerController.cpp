// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreeMatchPlayerController.h"
#include "ThreeMatchGame.h"

AThreeMatchPlayerController::AThreeMatchPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// We want the mouse cursor to show immediately on startup, without having to click in the window.
	bShowMouseCursor = true;

	// We need click/touch events to interact with our tiles.
	bEnableTouchEvents = bEnableClickEvents = true;
	bEnableTouchOverEvents = bEnableMouseOverEvents = true;

	ScoreChangeRate = 375.0f;
}

void AThreeMatchPlayerController::AddScore(int32 Points, bool bForceImmediateUpdate)
{
	Score += Points;
	if (bForceImmediateUpdate)
	{
		DisplayedScore = (float)Score;
	}
	else
	{
		GetWorldTimerManager().SetTimer(TickScoreDisplayHandle, this, &AThreeMatchPlayerController::TickScoreDisplay, 0.001f, true);
	}
}

int32 AThreeMatchPlayerController::GetScore()
{
	return Score;
}

int32 AThreeMatchPlayerController::GetDisplayedScore()
{
	return (int32)DisplayedScore;
}

int32 AThreeMatchPlayerController::CalculateBombPower_Implementation()
{
	return 0;
}

void AThreeMatchPlayerController::TickScoreDisplay()
{
	// This assumes score only goes up, or instantly drops when it is decreased.
	DisplayedScore += UGameplayStatics::GetWorldDeltaSeconds(this) * ScoreChangeRate;
	if (DisplayedScore >= (float)Score)
	{
		DisplayedScore = Score;
		GetWorldTimerManager().ClearTimer(TickScoreDisplayHandle);
	}
}