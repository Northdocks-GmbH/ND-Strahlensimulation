// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RayAbsorption.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URayAbsorption : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAYSIMULATION_API IRayAbsorption
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ray Simulation")
	void getAbsorbValues(float& absorb, float& reflect, float& pierce, float& pass);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ray Simulation")
	void addRadiation(float amount, UPrimitiveComponent* hitComponent, const FName hitBone = "NONE");
};
