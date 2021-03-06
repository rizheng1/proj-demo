// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// LightmassImportanceVolume:  a bounding volume outside of which Lightmass
// photon emissions are decreased
//=============================================================================
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Volume.h"
#include "LightmassImportanceVolume.generated.h"

UCLASS(hidecategories=(Collision, Brush, Attachment, Physics, Volume), MinimalAPI)
class ALightmassImportanceVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

	public:
		float mass = 100.0f;
		float horizon = 100.0f;
};



