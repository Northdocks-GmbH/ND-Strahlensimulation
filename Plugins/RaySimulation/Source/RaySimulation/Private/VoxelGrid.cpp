// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGrid.h"

// Sets default values
AVoxelGrid::AVoxelGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxelGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelGrid::SetVoxelNumber(const int32 xCount, const int32 yCount, const int32 zCount)
{
	xSize = xCount;
	ySize = yCount;
	zSize = zCount; 
	totalSize = xCount * yCount * zCount;
}

void AVoxelGrid::GetSizes(int32& xCount, int32& yCount, int32& zCount)
{
	xCount = xSize;
	yCount = ySize;
	zCount = zSize;
}

bool AVoxelGrid::IsValidPosition(const int32 position) const
{
	return (0 <= position && position < totalSize);
}

int32 AVoxelGrid::GetNeighbor(const int32 position, const EDirection direc)
{
	if (!IsValidPosition(position))
	{
		return -1;
	}

	int32 endPosition{};

	switch (direc)
	{
	case EDirection::FORWARD: endPosition = position + 1;
		break;
	case EDirection::BACK: endPosition = position - 1;
		break;
	case EDirection::RIGHT: endPosition = position + xSize;
		break;
	case EDirection::LEFT: endPosition = position - xSize;
		break;
	case EDirection::UP: endPosition = position + xSize * ySize;
		break;
	case EDirection::DOWN: endPosition = position - xSize * ySize;
		break;
	}

	return (IsValidPosition(endPosition) ? endPosition : -1);
}

int32 AVoxelGrid::GetPositionFromCoord(const int32 xCoord, const int32 yCoord, const int32 zCoord) const
{
	return xCoord + xSize*yCoord + xSize*ySize*zCoord;
}


void AVoxelGrid::GetCoordFromPosition(const int32 position, int32& xCoord, int32& yCoord, int32& zCoord)
{
	xCoord = position % xSize;
	yCoord = (position % (xSize*ySize)) / xSize;
	zCoord = position / (xSize * ySize);
}



TArray<int32> AVoxelGrid::GetVoxelLine(const int32 start, const int32 end) 
{
	//	Following Bresenham's Algorithm

	TArray<int32> result;

	int32 xStart{0};
	int32 yStart{0};
	int32 zStart{0};

	int32 xEnd{ xSize - 1 };
	int32 yEnd{ ySize - 1 };
	int32 zEnd{ zSize - 1 };


	if (IsValidPosition(start))
	{
		GetCoordFromPosition(start, xStart, yStart, zStart);
	}

	if (IsValidPosition(end))
	{
		GetCoordFromPosition(end, xEnd, yEnd, zEnd);
	}

	result.Add(start);

	if (start == end)
	{
		return result;
	}

	TArray<int32> differences{abs(xEnd - xStart), abs(yEnd - yStart), abs(zEnd - zStart) };
	TArray<EDirection> directions{	xEnd - xStart >= 0 ? EDirection::FORWARD : EDirection::BACK,
								yEnd - yStart >= 0 ? EDirection::RIGHT : EDirection::LEFT,
								zEnd - zStart >= 0 ? EDirection::UP : EDirection::DOWN };



	if (differences[0] < differences[1])
	{
		differences.Swap(0, 1);
		directions.Swap(0, 1);

	}

	if (differences[1] < differences[2])
	{
		differences.Swap(1, 2);
		directions.Swap(1, 2);


		if (differences[0] < differences[1])
		{
			differences.Swap(0, 1);
			directions.Swap(0, 1);
		}
	}




	int32 buffer[2]{ differences[1], differences[2] };
	int32 currentPosition{ start };

	while (currentPosition != end)
	{

		currentPosition = GetNeighbor(currentPosition, directions[0]);

		if (buffer[0] >= differences[0])
		{
			currentPosition = GetNeighbor(currentPosition, directions[1]);
			buffer[0] -= differences[0];
		}


		if (buffer[1] >= differences[0])
		{
			currentPosition = GetNeighbor(currentPosition, directions[2]);
			buffer[1] -= differences[0];
		}


		buffer[0] += differences[1];
		buffer[1] += differences[2];

		if (!IsValidPosition(currentPosition)) { break; }
		else { result.Add(currentPosition); }


	}


	return result;

}