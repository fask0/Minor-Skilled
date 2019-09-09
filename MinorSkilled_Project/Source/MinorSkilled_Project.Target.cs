// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MinorSkilled_ProjectTarget : TargetRules
{
	public MinorSkilled_ProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("MinorSkilled_Project");
	}
}
