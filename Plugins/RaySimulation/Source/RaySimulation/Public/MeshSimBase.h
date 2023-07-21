// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSimBase.generated.h"


UCLASS()
class RAYSIMULATION_API AMeshSimBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshSimBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "RaySimulation")
	virtual void ComputeSingleRayData(const FVector start, const FVector end, float& length, FVector& position, FQuat& rotation);

};
