// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreeMatchGameInstance.h"
#include "ThreeMatchGame.h"
#include "ThreeMatchSaveGame.h"
#include "UnrealClient.h"
#include "Online.h"
#include "OnlineSubsystem.h"

UThreeMatchGameInstance::UThreeMatchGameInstance()
{
	DefaultSaveGameSlot = TEXT("_ThreeMatchGame");
}

bool UThreeMatchGameInstance::FindSaveDataForLevel(UObject* WorldContextObject, FThreeMatchLevelSaveData& OutSaveData) const
{
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(WorldContextObject, true);
	if (FThreeMatchLevelSaveData* FoundData = InstanceGameData->ThreeMatchSaveData.Find(LevelName))
	{
		OutSaveData = *FoundData;
		return true;
	}
	return false;
}

void UThreeMatchGameInstance::UpdateSave(UObject* WorldContextObject, FThreeMatchLevelSaveData& NewData)
{
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(WorldContextObject, true);
	InstanceGameData->ThreeMatchSaveData.FindOrAdd(LevelName) = NewData;
	UpdateUIAfterSave();
}

void UThreeMatchGameInstance::SaveGame()
{
	UGameplayStatics::SaveGameToSlot(InstanceGameData, GetSaveSlotName(), 0);
}

bool UThreeMatchGameInstance::LoadCustomInt(FString FieldName, int32& Value)
{
	check(InstanceGameData);
	return InstanceGameData->LoadCustomInt(FieldName, Value);
}

void UThreeMatchGameInstance::SaveCustomInt(FString FieldName, int32 Value)
{
	check(InstanceGameData);
	InstanceGameData->SaveCustomInt(FieldName, Value);
}

void UThreeMatchGameInstance::ClearCustomInt(FString FieldName)
{
	check(InstanceGameData);
	InstanceGameData->ClearCustomInt(FieldName);
}

void UThreeMatchGameInstance::Init()
{
	// Point to a default save slot at startup. We will later change our save slot when we log in.
	InitSaveGameSlot();

	
	LoginChangedHandle = FCoreDelegates::OnUserLoginChangedEvent.AddUObject(this, &UThreeMatchGameInstance::OnLoginChanged);
	EnteringForegroundHandle = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UThreeMatchGameInstance::OnEnteringForeground);
	EnteringBackgroundHandle = FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UThreeMatchGameInstance::OnEnteringBackground);
	ViewportHandle = FViewport::ViewportResizedEvent.AddUObject(this, &UThreeMatchGameInstance::OnViewportResize_Internal);

	IOnlinePurchasePtr PurchaseInterface = Online::GetPurchaseInterface();
	if (PurchaseInterface.IsValid())
	{
		UnexpectedPurchaseHandle = PurchaseInterface->AddOnUnexpectedPurchaseReceiptDelegate_Handle(FOnUnexpectedPurchaseReceiptDelegate::CreateUObject(this, &UThreeMatchGameInstance::OnUnexpectedPurchase_Internal));
	}
	

	Super::Init();
}



void UThreeMatchGameInstance::Shutdown()
{
	FCoreDelegates::OnUserLoginChangedEvent.Remove(LoginChangedHandle);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Remove(EnteringForegroundHandle);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.Remove(EnteringBackgroundHandle);
	FViewport::ViewportResizedEvent.Remove(ViewportHandle);
	

	Super::Shutdown();
}

void UThreeMatchGameInstance::InitSaveGameSlot()
{
	const FString SaveSlotName = GetSaveSlotName();
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		// Clear default save file, if it exists.
		if (UGameplayStatics::DoesSaveGameExist(DefaultSaveGameSlot, 0))
		{
			UGameplayStatics::DeleteGameInSlot(DefaultSaveGameSlot, 0);
		}
		// If we have no save object, create one.
		if (InstanceGameData == nullptr)
		{
			// We're either not logged in with an Online ID, or we have no save data to transfer over (usually, this indicates program startup).
			InstanceGameData = Cast<UThreeMatchSaveGame>(UGameplayStatics::CreateSaveGameObject(UThreeMatchSaveGame::StaticClass()));
		}
		UGameplayStatics::SaveGameToSlot(InstanceGameData, SaveSlotName, 0);
	}
	else
	{
		InstanceGameData = Cast<UThreeMatchSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	check(InstanceGameData);
}

FString UThreeMatchGameInstance::GetSaveSlotName() const
{
	return SaveGamePrefix + DefaultSaveGameSlot;
}

void UThreeMatchGameInstance::RegisterOnlineID(FString NewOnlineID)
{
	SaveGamePrefix = NewOnlineID;
	InitSaveGameSlot();
}

void UThreeMatchGameInstance::OnViewportResize_Internal(FViewport* Viewport, uint32 ID)
{
	OnViewportResize();
}

void UThreeMatchGameInstance::OnUnexpectedPurchase_Internal(const FUniqueNetId& UserId)
{
	OnUnexpectedPurchase();
}