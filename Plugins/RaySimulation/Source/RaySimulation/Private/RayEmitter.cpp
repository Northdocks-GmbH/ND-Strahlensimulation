          // Fill out your copyright notice in the Description page of Project Settings.


#include "RayEmitter.h"

#define ECC_Radiation ECC_GameTraceChannel1


// Sets default values
ARayEmitter::ARayEmitter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComponent;
	SetActorTickInterval(0.29f);


}

// Called when the game starts or when spawned
void ARayEmitter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARayEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Generates a random Rotator with values between -MaxDeviation and MaxDeviation. 
// Normal weight just generates a random rotator, middle generates an averaged rotator, low generates lower values and high generates higher values. 
FRotator ARayEmitter::GetRandomRotator(const float MaxDeviation, const ARayEmitter::RandomWeight weight) const
{
	float pitch0 = FMath::FRandRange(-MaxDeviation, MaxDeviation);
	const float pitch1 = FMath::FRandRange(-MaxDeviation, MaxDeviation);
	float yaw0 = FMath::FRandRange(-MaxDeviation, MaxDeviation);
	const float yaw1 = FMath::FRandRange(-MaxDeviation, MaxDeviation);
	float roll0 = FMath::FRandRange(-MaxDeviation, MaxDeviation);
	const float roll1 = FMath::FRandRange(-MaxDeviation, MaxDeviation);

	switch (weight)
	{
	case RandomWeight::NORMAL: return FRotator(pitch0, yaw0, roll0);
	case RandomWeight::MIDDLE: return FRotator((pitch0 + pitch1) / 2, (yaw0 + yaw1) / 2, (roll0 + roll1) / 2);
	case RandomWeight::LOW: 

		if (abs(pitch0) > abs(pitch1)) { pitch0 = pitch1; }
		if (abs(yaw0) > abs(yaw1)) { yaw0 = yaw1; }
		if (abs(roll0) > abs(roll1)) { roll0 = roll1; }
		return FRotator(pitch0, yaw0, roll0);

	case RandomWeight::HIGH:

		if (abs(pitch0) < abs(pitch1)) { pitch0 = pitch1; }
		if (abs(yaw0) < abs(yaw1)) { yaw0 = yaw1; }
		if (abs(roll0) < abs(roll1)) { roll0 = roll1; }
		return FRotator(pitch0, yaw0, roll0);

	}
	
	return FRotator(pitch0, yaw0, roll0);
}

// This function is only here to be exposed to blueprints since you can't work with quaternions otherwise. 
FQuat ARayEmitter::RotatorToQuat(const FRotator inRot) const
{
	return inRot.Quaternion();
}


// Rotates a given vector by a random rotator. The higher maxDeviation, the more the vector gets turned.  
FVector ARayEmitter::GetRotatedVectorRandom(const FVector InVector, const float maxDeviation, const RandomWeight inWeight) const
{
	FRotator tempRot{ InVector.ToOrientationRotator() };
	tempRot = UKismetMathLibrary::ComposeRotators(tempRot, GetRandomRotator(maxDeviation, inWeight));

	return tempRot.Vector();
}



// Creates a random behaviour out of RayBehaviour. 
RayBehaviour ARayEmitter::GetBehaviour(float AbsorbChance = 0.5f, float ReflectChance = 0.5f, float PierceChance = 0.0f, float PassChance = 0.0f)
{
	const float randomFloat{ FMath::FRandRange(0, 1) };
	
	if (randomFloat < AbsorbChance)
	{
		return RayBehaviour::ABSORB;
	}
	if (randomFloat < AbsorbChance + ReflectChance)
	{
		return RayBehaviour::REFLECT;
	}
	if (randomFloat < AbsorbChance + ReflectChance + PierceChance)
	{
		return RayBehaviour::PIERCE;
	}
	return RayBehaviour::PASS;
}


// This function emits a single ray. 
void ARayEmitter::EmitRay(TArray<FVector>& outLines, TArray<float>& outRadiation, RayBehaviour& firstRefraction, bool bPrintLines = true)
{
	float RemainingTraceDistance{ TraceDistance };
	float RemainingRadiation{ StartRadiation/RayNumber };

	FVector Start = GetActorLocation();
	FVector Direction = (UKismetMathLibrary::ComposeRotators(GetActorRotation(), GetRandomRotator(MaxAngleDeviation, RandomWeight::NORMAL))).Vector();
	FVector End = Start + (Direction * RemainingTraceDistance);

	outLines.Add(Start);
	outRadiation.Add(RemainingRadiation);

	firstRefraction = RayBehaviour::PASS;

	FHitResult Hit;
	FCollisionQueryParams TraceParams;


	while (RemainingTraceDistance > 1.0f)
	{
		bool bHit{ GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Radiation, TraceParams) };
		RayBehaviour behav;
		bool bCheckAbsorb{ false };
		
		RemainingRadiation *= FMath::Min(1.0f, FMath::Square(RadiationIdenticalDistance/Hit.Distance));

		if (bHit)
		{

			if (bPrintLines)
			{
				DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(10, 10, 10), FColor::Emerald, false, GetActorTickInterval());

				DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Red, false, GetActorTickInterval());
			}

			outLines.Add(Hit.ImpactPoint);

			bCheckAbsorb = Hit.Actor->Implements<URayAbsorption>();


			if (bCheckAbsorb)
			{
				float absorb{};
				float reflect{};
				float pierce{};
				float pass{};

				IRayAbsorption::Execute_getAbsorbValues(Hit.GetActor(), absorb, reflect, pierce, pass);

				behav = GetBehaviour(absorb, reflect, pierce, pass);

			}
			else
			{
				behav = GetBehaviour();
			}


			switch (behav)
			{
			case RayBehaviour::REFLECT:

				RemainingTraceDistance -= (Start - Hit.ImpactPoint).Size();

				Start = Hit.ImpactPoint;
				Direction = -Direction.RotateAngleAxis(180.0f, Hit.ImpactNormal);

				Direction = GetRotatedVectorRandom(Direction, 90.0f, RandomWeight::HIGH);

				End = Start + RemainingTraceDistance * Direction;

				TraceParams.ClearIgnoredActors();
				TraceParams.AddIgnoredActor(Hit.GetActor());

				if (bCheckAbsorb)
				{
					IRayAbsorption::Execute_addRadiation(Hit.GetActor(), RemainingRadiation * RadiationOnReflect, Hit.GetComponent(), Hit.BoneName);
				}

				RemainingRadiation = RemainingRadiation * (1 - RadiationOnReflect);
				outRadiation.Add(RemainingRadiation);

				break;

			case RayBehaviour::PIERCE:

				RemainingTraceDistance -= (Start - Hit.ImpactPoint).Size();
				Start = Hit.ImpactPoint;
				Direction = GetRotatedVectorRandom(Direction, 90.0f, RandomWeight::HIGH);

				End = Start + RemainingTraceDistance * Direction;

				TraceParams.ClearIgnoredActors();
				TraceParams.AddIgnoredActor(Hit.GetActor());

				if (bCheckAbsorb)
				{
					IRayAbsorption::Execute_addRadiation(Hit.GetActor(), RemainingRadiation * RadiationOnPierce, Hit.GetComponent(), Hit.BoneName);
				}

				RemainingRadiation = RemainingRadiation * (1 - RadiationOnPierce);
				outRadiation.Add(RemainingRadiation);


				break;


			case RayBehaviour::PASS:

				RemainingTraceDistance -= (Start - Hit.ImpactPoint).Size();
				Start = Hit.ImpactPoint;

				outRadiation.Add(RemainingRadiation);

				TraceParams.ClearIgnoredActors();
				TraceParams.AddIgnoredActor(Hit.GetActor());

				break;



			case RayBehaviour::ABSORB:

				RemainingTraceDistance = 0.0f;
				outRadiation.Add(0.0f);

				if (bCheckAbsorb)
				{
					IRayAbsorption::Execute_addRadiation(Hit.GetActor(), RemainingRadiation, Hit.GetComponent(), Hit.BoneName);
				}
			
				break;
			}

			if (firstRefraction == RayBehaviour::PASS)
			{
				firstRefraction = behav;
			}



		}
		else
		{
			RemainingTraceDistance = 0.0f;
			outLines.Add(End);
			outRadiation.Add(0.0f);

			if (bPrintLines)
			{
				DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, GetActorTickInterval());
			}
		}
	}

}



