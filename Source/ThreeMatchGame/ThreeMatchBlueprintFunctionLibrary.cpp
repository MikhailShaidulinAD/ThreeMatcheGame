// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreeMatchBlueprintFunctionLibrary.h"
#include "ThreeMatchGame.h"
#include "ThreeMatchGameMode.h"

APlayerController* UThreeMatchBlueprintFunctionLibrary::GetLocalPlayerController(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController && PlayerController->IsLocalController())
			{
				// For this project, we will only ever have one local player.
				return PlayerController;
			}
		}
	}
	return nullptr;
}

FString UThreeMatchBlueprintFunctionLibrary::GetOnlineAccountID(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->PlayerState && PlayerController->PlayerState->GetUniqueId().IsValid())
	{
		return PlayerController->PlayerState->GetUniqueId()->GetHexEncodedString();
	}
	return FString();
}

bool UThreeMatchBlueprintFunctionLibrary::IsGameActive(UObject* WorldContextObject)
{
	if (AThreeMatchGameMode* GameMode = Cast<AThreeMatchGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		if (GameMode->IsGameActive())
		{
			return true;
		}
	}
	return false;
}

void UThreeMatchBlueprintFunctionLibrary::PauseGameTimer(UObject* WorldContextObject, bool bPause)
{
	if (AThreeMatchGameMode* GameMode = Cast<AThreeMatchGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		GameMode->PauseGameTimer(bPause);
	}
}