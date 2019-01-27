// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlueNoiseBPLibrary.h"
#include "BlueNoise.h"


UBlueNoiseBPLibrary::UBlueNoiseBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FVector2D UBlueNoiseBPLibrary::NewPoint(FCoordinates& arr)
{
	TArray<FVector2D> candidates;
	for (int i = 0; i < arr.x.Num()*100; i++) {
		candidates.Add(FVector2D(arr.rand.RandRange(-4000.0, 4000.0), arr.rand.RandRange(-4000.0, 4000.0)));
	}

	FVector2D best;
	float bestdist = 0;
	float closest = FVector2D::Distance(arr.x[0], candidates[0]);
	float dist;

	for (int j = 0; j < candidates.Num(); j++) {

		for (int k = 0; k < arr.x.Num(); k++) {
			dist = FVector2D::Distance(candidates[j], arr.x[k]);
			if (dist < closest) {
				closest = dist;
			}
		}

		if (bestdist < closest) {
			best = candidates[j];
			bestdist = closest;
		}
	}

	arr.x.Add(best);
	return best;	
}

FCoordinates UBlueNoiseBPLibrary::Initialize(int seed, float min, float max) {

	FRandomStream stream;
	stream.Initialize(seed);

	FCoordinates data;
	data.rand = stream;
	data.x.Add(FVector2D(0.0f, 0.0f));
	return data;
}
