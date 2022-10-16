// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "ThreeMatchSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FThreeMatchLevelSaveData
{
	GENERATED_USTRUCT_BODY()

	/** Score to beat to get gold. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GoldScore;

	/** Score to beat to get silver. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SilverScore;

	/** Score to beat to get bronze. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BronzeScore;

	/** Player's personal best score. Not necessarily a gold-medal score. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TopScore;
};

/**
 * 
 */
UCLASS()
class THREEMATCHGAME_API UThreeMatchSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/**
	*	@see UGameplayStatics::CreateSaveGameObject
	*	@see UGameplayStatics::SaveGameToSlot
	*	@see UGameplayStatics::DoesSaveGameExist
	*	@see UGameplayStatics::LoadGameFromSlot
	*	@see UGameplayStatics::DeleteGameInSlot
	*/

	/** Complete list of levels played in ThreeMatch mode. */
	UPROPERTY()
	TMap<FString, FThreeMatchLevelSaveData> ThreeMatchSaveData;


	//~
	//~ Custom global fields for saved games.
	//~

	/** Load the int32 value associated with the requested variable. */
	bool LoadCustomInt(FString FieldName, int32& Value) const;

	/** Create a variable in the saved game and associate the provided integer value with it. */
	void SaveCustomInt(FString FieldName, int32 Value);

	/** Erase a variable from the saved game. */
	void ClearCustomInt(FString FieldName);

protected:
	UPROPERTY()
	TMap<FString, int32> ThreeMatchCustomIntData;
};
