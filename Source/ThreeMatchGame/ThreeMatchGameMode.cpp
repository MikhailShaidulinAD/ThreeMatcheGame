// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreeMatchGameMode.h"
#include "ThreeMatchGameInstance.h"
#include "ThreeMatchSaveGame.h"
#include "ThreeMatchGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "ThreeMatchPlayerController.h"


AThreeMatchGameMode::AThreeMatchGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass =  APawn::StaticClass();
	PlayerControllerClass = AThreeMatchPlayerController::StaticClass();
	TileMoveSpeed = 50.0f;
	TimeRemaining = 5.0f;
	FinalPlace = 0;
}

void AThreeMatchGameMode::BeginPlay()
{
	Super::BeginPlay();
	bGameWillBeWon = false;
	ChangeMenuWidget(StartingWidgetClass);
	GetWorldTimerManager().SetTimer(GameOverTimer, this, &AThreeMatchGameMode::GameOver, TimeRemaining, false);

	// Get our current save data from the game instance.
	UThreeMatchGameInstance* GameInstance = Cast<UThreeMatchGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		// If we didn't already have save data, put our defaults into the main array. We'll save it later, if anything noteworthy happens.
		if (!GameInstance->FindSaveDataForLevel(this, SaveGameData))
		{
			GameInstance->UpdateSave(this, SaveGameData);
		}
	}
}

void AThreeMatchGameMode::GameRestart()
{
	ChangeMenuWidget(nullptr);
	FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, LevelName);
}

void AThreeMatchGameMode::GameOver()
{
	GetWorldTimerManager().ClearTimer(GameOverTimer);

	if (bGameWillBeWon)
	{
		UThreeMatchGameInstance* GameInstance = Cast<UThreeMatchGameInstance>(UGameplayStatics::GetGameInstance(this));
		// Check for top score
		if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
		{
			SaveGameData.TopScore = FMath::Max(PC->GetScore(), SaveGameData.TopScore);
		}
		// Save regardless of whether or not we got a high score, because we save things like number of games played.
		GameInstance->UpdateSave(this, SaveGameData);
		GameInstance->SaveGame();
	}
	ChangeMenuWidget(bGameWillBeWon ? VictoryWidgetClass : DefeatWidgetClass);
	GameWasWon(bGameWillBeWon);
}

bool AThreeMatchGameMode::IsGameActive() const
{
	// Game is active whenever time hasn't run out or the timer is paused.
	FTimerManager& WorldTimerManager = GetWorldTimerManager();
	return (WorldTimerManager.IsTimerActive(GameOverTimer) || WorldTimerManager.IsTimerPaused(GameOverTimer));
}

void AThreeMatchGameMode::PauseGameTimer(bool bPause)
{
	if (bPause)
	{
		GetWorldTimerManager().PauseTimer(GameOverTimer);
	}
	else
	{
		GetWorldTimerManager().UnPauseTimer(GameOverTimer);
	}
}

FString AThreeMatchGameMode::GetRemainingTimeAsString()
{
	int32 OutInt = FMath::CeilToInt(GetWorldTimerManager().GetTimerRemaining(GameOverTimer));
	return FString::Printf(TEXT("%03i"), FMath::Max(0, OutInt));
}


bool AThreeMatchGameMode::GetTimerPaused()
{
	return GetWorldTimerManager().IsTimerPaused(GameOverTimer);
}

void AThreeMatchGameMode::AddScore(int32 Points)
{
	if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
	{
		int32 OldScore = PC->GetScore();
		PC->AddScore(Points);
		int32 NewScore = PC->GetScore();
		if (NewScore >= SaveGameData.BronzeScore)
		{
			bGameWillBeWon  = true;
		}

		// Check for medals
		if (NewScore > SaveGameData.GoldScore)
		{
			FinalPlace = 1;
			AwardPlace(1, Points);
		}
		else if (NewScore > SaveGameData.SilverScore)
		{
			FinalPlace = 2;
			AwardPlace(2, Points);
		}
		else if (NewScore > SaveGameData.BronzeScore)
		{
			FinalPlace = 3;
			AwardPlace(3, Points);
		}
		else
		{
			FinalPlace = 0;
			AwardPlace(0, Points);
		}
		
		for (const FThreeMatchReward& Reward : Rewards)
		{
			check(Reward.ScoreInterval > 0);
			// Integer division to decide if we've crossed a bonus threshold
			int32 ScoreAwardCount = (NewScore / Reward.ScoreInterval) - (OldScore / Reward.ScoreInterval);
			if (ScoreAwardCount > 0)
			{
				float StartingTimeValue = GetWorldTimerManager().GetTimerRemaining(GameOverTimer);
				if (StartingTimeValue >= 0.0f)
				{
					GetWorldTimerManager().SetTimer(GameOverTimer, this, &AThreeMatchGameMode::GameOver, StartingTimeValue + (ScoreAwardCount * Reward.TimeAwarded), false);
					AwardBonus();
				}
			}
		}
	}
}

void AThreeMatchGameMode::UpdateScoresFromLeaderBoard(int32 GoldScore, int32 SilverScore, int32 BronzeScore)
{
	UThreeMatchGameInstance* GameInstance = Cast<UThreeMatchGameInstance>(UGameplayStatics::GetGameInstance(this));
	
	SaveGameData.BronzeScore = BronzeScore;
	SaveGameData.SilverScore = SilverScore;
	SaveGameData.GoldScore = GoldScore;

	GameInstance->SaveGame();
}

void AThreeMatchGameMode::SetComboPower(int32 NewComboPower)
{
	if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
	{
		PC->ComboPower = NewComboPower;
	}
}

int32 AThreeMatchGameMode::GetComboPower()
{
	if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
	{
		return PC->ComboPower;
	}
	return 0;
}

int32 AThreeMatchGameMode::GetMaxComboPower()
{
	if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
	{
		return PC->MaxComboPower;
	}
	return 0;
}

int32 AThreeMatchGameMode::CalculateBombPower_Implementation()
{
	if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
	{
		return PC->CalculateBombPower();
	}
	return 0;
}

void AThreeMatchGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass)
	{
		if (AThreeMatchPlayerController* PC = Cast<AThreeMatchPlayerController>(UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(this)))
		{
			CurrentWidget = CreateWidget<UUserWidget>(PC, NewWidgetClass);
			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
			}
		}
	}
}
