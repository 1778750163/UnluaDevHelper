// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RegistryManager.h"
#include "UnluaDevHelperSetting.h"
#include "IDEAPathLocator/IDEAPathLocator.h"
#include "Modules/ModuleManager.h"

struct lua_State;
class FDHMainMenuBar;
class FDHBlueprinBar;

class FUnluaDevHelperEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();
	
	void OpenSolution(const FString& FileName=TEXT(""));

	void ChangeIDE(EIDEType InIDEType);
	
private:

	void StartDebug(const bool bStart);
	
	void EnableVSCodeDebug(lua_State *L);

	void VSCodeOpenSolution(const FString& FileName=TEXT(""));

	void EnableIDEADebug(lua_State *L);
	void IdeaOpenSolution(const FString& FileName=TEXT(""));
	void RefreshAvailability();

private:
	EIDEType IDEType;
	FString Host;
	int32 VSCodePort;
	int32 IDEAPort;
	FInstallInfo IdeaInstallInfo;

	TSharedPtr<FDHMainMenuBar> MainMenuBar;
	TSharedPtr<FDHBlueprinBar> DHBlueprinBar;

private:
	struct FLocation
	{
		bool IsValid() const
		{
			return URL.Len() > 0;
		}

		FString URL;
	};
	FLocation Location;
};


