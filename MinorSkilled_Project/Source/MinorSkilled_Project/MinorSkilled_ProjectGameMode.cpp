// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MinorSkilled_ProjectGameMode.h"
#include "MinorSkilled_ProjectCharacter.h"

AMinorSkilled_ProjectGameMode::AMinorSkilled_ProjectGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = AMinorSkilled_ProjectCharacter::StaticClass();
}
