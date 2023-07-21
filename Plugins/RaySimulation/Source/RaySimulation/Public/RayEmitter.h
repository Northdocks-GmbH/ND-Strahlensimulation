// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"


#include "RayAbsorption.h"

#include "RayEmitter.generated.h"



UENUM(BlueprintType)
enum class RayBehaviour: uint8
{
	ABSORB UMETA(DisplayName = "Absorb"),
	REFLECT UMETA(DisplayName = "Reflect"),
	PIERCE UMETA(DisplayName = "Pierce"),
	PASS UMETA(DisplayName = "Pass")
};


UCLASS()
class RAYSIMULATION_API ARayEmitter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARayEmitter();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	enum class RandomWeight
	{
		NORMAL,
		LOW,
		MIDDLE,
		HIGH
	};

	FRotator GetRandomRotator(const float MaxDeviation, const ARayEmitter::RandomWeight weight = RandomWeight::NORMAL) const; // Generates a random rotator within maxDeviation. weight can be used to get a tendency towards particularly high/low results.
	RayBehaviour GetBehaviour(float AbsorbChance, float ReflectChance, float PierceChance, float PassChance); // Gets a random behaviour to determine if a ray should pierce, be absorbed or be reflected
	FVector GetRotatedVectorRandom(const FVector InVector, const float maxDeviation, const RandomWeight inWeight = RandomWeight::NORMAL) const;  // Returns a randomly rotated vector within maxDeviation. RandomWeight is passed on to GetRandomRotator.

public:

	UPROPERTY(Category = "RayEmission", VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(Category = "RayEmission", EditAnywhere, BlueprintReadWrite)
	float TraceDistance = 2000.0f; // The maximal lenght of a single ray

	UPROPERTY(Category = "RayEmission", EditAnywhere, BlueprintReadWrite, meta=(ClampMin = "0.0", ClampMax = "180.0"))
	float MaxAngleDeviation = 10.0f; // Defines the spread of the rays

	UPROPERTY(Category = "RayEmission", EditAnywhere, BlueprintReadWrite)
	int32 RayNumber = 10; // Number of rays emitted per tick

	UPROPERTY(Category = "RayEmission", EditAnywhere, BlueprintReadWrite)
	float StartRadiation = 100; // The radiation per second that this emits

	float RadiationOnReflect{ 0.05f }; // How much Radiation an object absorbs when it reflects a ray
	float RadiationOnPierce{ 0.5f }; // How much Radiation an object absorbs when it is pierced by a ray

	UPROPERTY(Category = "RayEmission", EditAnywhere, BlueprintReadOnly)
	float RadiationIdenticalDistance{ 50.0f }; // Within this distance, the radiation absorbed is considered 100% of the radiation emitted. After this distance, the radiation absorbed falls of quadratically. The default is 50 cm.


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Emission")
	void EmitRay(TArray<FVector>& outLines, TArray<float>& outRadiation, RayBehaviour& firstRefraction, bool bPrintLines);

	UFUNCTION(BlueprintCallable, Category = "Math")
	FQuat RotatorToQuat(const FRotator inRot) const;

};
