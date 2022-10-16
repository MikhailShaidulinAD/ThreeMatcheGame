#include "SaveGame.h"

bool UMSaveGame::LoadCustomInt(FString FieldName, int32& Value) const
{
	const int32* ValuePointer = CustomIntData.Find(FieldName);
	if (ValuePointer != nullptr)
	{
		Value = *ValuePointer;
		return true;
	}
	return false;
}

void UMSaveGame::SaveCustomInt(FString FieldName, int32 Value)
{
	CustomIntData.FindOrAdd(FieldName) = Value;
}

void UMSaveGame::ClearCustomInt(FString FieldName)
{
	CustomIntData.Remove(FieldName);
}