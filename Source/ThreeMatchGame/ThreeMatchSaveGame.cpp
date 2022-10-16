// Copyright Epic Games, Inc. All Rights Reserved.
#include "ThreeMatchSaveGame.h"
#include "ThreeMatchGame.h"
#include "Kismet/GameplayStatics.h"

bool UThreeMatchSaveGame::LoadCustomInt(FString FieldName, int32& Value) const
{
	const int32* ValuePointer = ThreeMatchCustomIntData.Find(FieldName);
	if (ValuePointer != nullptr)
	{
		Value = *ValuePointer;
		return true;
	}
	return false;
}

void UThreeMatchSaveGame::SaveCustomInt(FString FieldName, int32 Value)
{
	ThreeMatchCustomIntData.FindOrAdd(FieldName) = Value;
}

void UThreeMatchSaveGame::ClearCustomInt(FString FieldName)
{
	ThreeMatchCustomIntData.Remove(FieldName);
}