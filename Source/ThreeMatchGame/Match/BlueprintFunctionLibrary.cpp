#include "BlueprintFunctionLibrary.h"


// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

APlayerController* UMBlueprintFunctionLibrary::GetLocalPlayerController(UObject* WorldContextObject)
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

FString UMBlueprintFunctionLibrary::GetOnlineAccountID(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->PlayerState && PlayerController->PlayerState->GetUniqueId().IsValid())
	{
		return PlayerController->PlayerState->GetUniqueId()->GetHexEncodedString();
	}
	return FString();
}

bool UMBlueprintFunctionLibrary::IsGameActive(UObject* WorldContextObject)
{
	if (AMGameMode* GameMode = Cast<AMGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		if (GameMode->IsGameActive())
		{
			return true;
		}
	}
	return false;
}

void UMBlueprintFunctionLibrary::PauseGameTimer(UObject* WorldContextObject, bool bPause)
{
	if (AMGameMode* GameMode = Cast<AMGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		GameMode->PauseGameTimer(bPause);
	}
}