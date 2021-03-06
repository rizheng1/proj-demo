// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Lightmass/LightmassGravityVolume.h"
#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"


ALightmassGravityVolume::ALightmassGravityVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	bColored = true;
	BrushColor.R = 255;
	BrushColor.G = 255;
	BrushColor.B = 25;
	BrushColor.A = 255;

}

