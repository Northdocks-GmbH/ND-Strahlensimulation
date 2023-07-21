// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshSimBase.h"

// Sets default values
AMeshSimBase::AMeshSimBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMeshSimBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeshSimBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeshSimBase::ComputeSingleRayData(const FVector start, const FVector end, float& length, FVector& position, FQuat& rotation)
{
	position = 0.5f * (start + end);
	length = (start - end).Size();
	rotation = FRotationMatrix::MakeFromZ(start - end).ToQuat();

}

