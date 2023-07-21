// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelGrid.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	FORWARD UMETA(DisplayName = "Forward"),
	BACK UMETA(DisplayName = "Back"),
	RIGHT UMETA(DisplayName = "Right"),
	LEFT UMETA(DisplayName = "Left"),
	UP UMETA(DisplayName = "Up"),
	DOWN UMETA(DisplayName = "Down"),

};


UCLASS()
class RAYSIMULATION_API AVoxelGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelGrid();


	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:
	//Exposed Variables



	//Non-Exposed Variables

	int32 xSize = 10;
	int32 ySize = 10;
	int32 zSize = 10;
	int32 totalSize = xSize * ySize * zSize;

	float xLength = 1.0f;
	float yLength = 1.0f;
	float zLength = 1.0f;




public:  

	//Exposed functions

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetVoxelNumber(const int32 xCount, const int32 yCount, const int32 zCount);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<int32> GetVoxelLine(const int32 start, const int32 end);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	int32 GetVoxelCount() const { return totalSize; }

	UFUNCTION(BlueprintCallable, Category = "Grid")
	int32 GetPositionFromCoord(const int32 xCoord, const int32 yCoord, const int32 zCoord) const;

	UFUNCTION(BlueprintCallable, Category = "Grid", BlueprintPure)
	void GetSizes(int32& xCount, int32& yCount, int32& zCount);

	
//Non-Exposed functions

	int32 GetNeighbor(const int32 position, const EDirection direc);

	void GetCoordFromPosition(const int32 position, int32& xCoord, int32& yCoord, int32& zCoord);
	bool IsValidPosition(const int32 position) const;



};
