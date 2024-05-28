// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnluaDevHelperEditor.h"
#include <iostream>
#include <winreg.h>

#include "Editor.h"
#include "ISettingsModule.h"
#include "lauxlib.h"
#include "LevelEditor.h"
#include "lua.h"
#include "Regex.h"
#include "UnluaDevHelperDefine.h"
#include "UnluaDevHelperSetting.h"
#include "UnluaDevHelperStyle.h"
#include "BlueprintBar/DHBlueprinBar.h"
#include "IDEAPathLocator/IDEAPathLocator.h"
#include "Interfaces/IPluginManager.h"
#include "MenuBar/DHMainMenuBar.h"


#define LOCTEXT_NAMESPACE "FUnluaDevHelperModule"

void FUnluaDevHelperEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FUnluaDevHelperStyle::GetInstance();
	
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
		if(Setting->LuaFileDirectory.Len()==0)
		{
			Setting->SetLuaFileDirectory();
		}
		SettingsModule->RegisterSettings("Project", "Plugins", "UnluaDevHelper",
			LOCTEXT("UnluaDevHelper", "UnluaDevHelper"),
			LOCTEXT("UnluaDevHelper", "UnluaDevHelper some setting"),
			GetMutableDefault<UUnluaDevHelperSetting>()
		);
	}
	
	auto& Settings = *GetDefault<UUnluaDevHelperSetting>();
	IDEType=Settings.IDEType;
	VSCodePort=8818;
	IDEAPort=9966;
	Host=TEXT("127.0.0.1");

	TArray<FInstallInfo> InstallInfos =FIDEAPathLocator::CollectAllPaths().Array();
	if(InstallInfos.Num())
	{
		IdeaInstallInfo=InstallInfos.Last();
	}

	RefreshAvailability();
	
	FEditorDelegates::BeginPIE.AddRaw(this,&FUnluaDevHelperEditorModule::StartDebug);

	MainMenuBar=MakeShareable(new FDHMainMenuBar);
	MainMenuBar->Init();

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FUnluaDevHelperEditorModule::OnPostEngineInit);

}

void FUnluaDevHelperEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorDelegates::BeginPIE.RemoveAll(this);
	MainMenuBar->Clear();
	MainMenuBar=nullptr;

	DHBlueprinBar->Clear();
	DHBlueprinBar=nullptr;

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "UnluaDevHelper");
	}
}

void FUnluaDevHelperEditorModule::OnPostEngineInit()
{
	DHBlueprinBar=MakeShareable(new FDHBlueprinBar);
	DHBlueprinBar->Init();
}

void FUnluaDevHelperEditorModule::OpenSolution(const FString& FileName)
{
	switch (IDEType) {
	case EIDEType::VSCode:
		{
			VSCodeOpenSolution(FileName);
		}
		break;
	case EIDEType::IDEA:
		{
			IdeaOpenSolution(FileName);
		}
		break;
	}
}

void FUnluaDevHelperEditorModule::ChangeIDE(EIDEType InIDEType)
{
	IDEType=InIDEType;
	auto& Settings = *GetMutableDefault<UUnluaDevHelperSetting>();
	Settings.IDEType=InIDEType;
	Settings.UpdateProjectUserConfigFile();
}

void FUnluaDevHelperEditorModule::StartDebug(const bool bStart)
{
	TMap<lua_State*, UnLua::FLuaEnv*>&  EnvMap=UnLua::FLuaEnv::GetAll();
	for (auto Evn:EnvMap)
	{
		switch (IDEType) {
		case EIDEType::VSCode:
			{
				EnableVSCodeDebug(Evn.Key);
			}
			break;
		case EIDEType::IDEA:
			{
				EnableIDEADebug(Evn.Key);
			}
			break;
		}
	}
}

void FUnluaDevHelperEditorModule::EnableVSCodeDebug(lua_State *L)
{
	FString luaPanda =TEXT("UnLua.PackagePath = UnLua.PackagePath..';/Plugins/UnluaDevHelper/Config/?.lua'");
	if (luaL_dostring(L, TCHAR_TO_ANSI(*luaPanda)) != LUA_OK) {
		lua_pop(L, 1);
	}

	FString luaCode =FString::Printf( TEXT("local LuaPanda = require('LuaPanda')\n LuaPanda.start('%s',%d)"),*Host,VSCodePort);
	if (luaL_dostring(L, TCHAR_TO_ANSI(*luaCode)) != LUA_OK) {
		lua_pop(L, 1);
	}
}


void FUnluaDevHelperEditorModule::VSCodeOpenSolution(const FString& FileName)
{
	auto& Settings = *GetDefault<UUnluaDevHelperSetting>();
	
	FString ScriptPath=FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(),Settings.LuaFileDirectory));
	FString FilePath=ScriptPath;
	if(FileName.Len())
	{
		FilePath=FString::Printf(TEXT("%s  %s"),*FilePath,*FPaths::Combine(FilePath,FileName));
	}

	if (Location.IsValid())
	{
		if (Location.IsValid())
		{
			uint32 ProcessID;
			FProcHandle hProcess = FPlatformProcess::CreateProc(*Location.URL, *FilePath, true, false, false,
			                                                    &ProcessID, 0, nullptr, nullptr, nullptr);
		}
	}
}

void FUnluaDevHelperEditorModule::EnableIDEADebug(lua_State *L)
{
	const char* userProfile = std::getenv("USERPROFILE");
	if(userProfile==nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("not find USERPROFILE"));
		return;
	}
	// set package.cpath
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "cpath");
	const char* currentCpath = lua_tostring(L, -1);
	FString DllPath=FString::Printf(TEXT("/AppData/Roaming/JetBrains/IntelliJIdea%s/plugins/EmmyLua/debugger/emmy/windows/x64/?.dll"),*IdeaInstallInfo.V);
	const std::string newCpath = std::string(currentCpath) + ";"+userProfile+TCHAR_TO_ANSI(*DllPath);
	lua_pop(L, 1);
	lua_pushstring(L, newCpath.c_str());
	lua_setfield(L, -2, "cpath");
	lua_pop(L, 1);
	
	FString luaCode =FString::Printf( TEXT("local dbg = require('emmy_core') \n dbg.tcpConnect('%s', %d)"),*Host,IDEAPort);
	if (luaL_dostring(L, TCHAR_TO_ANSI(*luaCode)) != LUA_OK) {
		lua_pop(L, 1);
	}
}

void FUnluaDevHelperEditorModule::IdeaOpenSolution(const FString& FileName)
{
	auto& Settings = *GetDefault<UUnluaDevHelperSetting>();
	FString ScriptPath=FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(),Settings.LuaFileDirectory));
	FString FilePath=ScriptPath;
	if(FileName.Len())
	{
		FilePath=FString::Printf(TEXT("%s  %s"),*FilePath,*FPaths::Combine(FilePath,FileName));
	}

	FProcHandle Proc = FPlatformProcess::CreateProc(*IdeaInstallInfo.Path, *FilePath, true, true, false, nullptr, 0,
												nullptr, nullptr);
	const bool bResult = Proc.IsValid();
	if (!bResult)
	{
		FPlatformProcess::CloseProc(Proc);
	}
}


void FUnluaDevHelperEditorModule::RefreshAvailability()
{
#if PLATFORM_WINDOWS
	FString IDEPath;

	if (!FWindowsPlatformMisc::QueryRegKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Classes\\Applications\\Code.exe\\shell\\open\\command\\"), TEXT(""), IDEPath))
	{
		FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Classes\\Applications\\Code.exe\\shell\\open\\command\\"), TEXT(""), IDEPath);
	}

	FString PatternString(TEXT("\"(.*)\" \".*\""));
	FRegexPattern Pattern(PatternString);
	FRegexMatcher Matcher(Pattern, IDEPath);
	if (Matcher.FindNext())
	{
		FString URL = Matcher.GetCaptureGroup(1);
		if (FPaths::FileExists(URL))
		{
			Location.URL = URL;
		}
	}
#elif PLATFORM_LINUX
	FString OutURL;
	int32 ReturnCode = -1;

	FPlatformProcess::ExecProcess(TEXT("/bin/bash"), TEXT("-c \"type -p code\""), &ReturnCode, &OutURL, nullptr);
	if (ReturnCode == 0)
	{
		Location.URL = OutURL.TrimStartAndEnd();
	}
	else
	{
		// Fallback to default install location
		FString URL = TEXT("/usr/bin/code");
		if (FPaths::FileExists(URL))
		{
			Location.URL = URL;
		}
	}
#elif PLATFORM_MAC
	NSURL* AppURL = [[NSWorkspace sharedWorkspace] URLForApplicationWithBundleIdentifier:@"com.microsoft.VSCode"];
	if (AppURL != nullptr)
	{
		Location.URL = FString([AppURL path]);
	}
#endif
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnluaDevHelperEditorModule, UnluaDevHelperEditor)