

#include "MenuBar/DHMainMenuBar.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "LevelEditor.h"
#include "MainMenuBarCommands.h"
#include "UnluaDevHelperEditor.h"

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 23
	#include "ToolMenu.h"
	#include "ToolMenuDelegates.h"
	#include "ToolMenus.h"
#else
	#include "Framework/MultiBox/MultiBoxBuilder.h"
#endif

#define LOCTEXT_NAMESPACE "FDHMainMenuBar"

FDHMainMenuBar::FDHMainMenuBar()
    :CommandList(new FUICommandList)
{
    FMainMenuBarCommands::Register();
    CommandList->MapAction(FMainMenuBarCommands::Get().OpenSoulation,FExecuteAction::CreateRaw(this,&FDHMainMenuBar::OnOpenSoulation));
    
    CommandList->MapAction(FMainMenuBarCommands::Get().EnableDebug,
                           FExecuteAction::CreateStatic(&FDHMainMenuBar::OnEnableDebug),
                           FCanExecuteAction(),
                           FIsActionChecked::CreateStatic(&FDHMainMenuBar::IsEnableDebugCheck));
    
    CommandList->MapAction(FMainMenuBarCommands::Get().SelectVSCode,
                           FExecuteAction::CreateStatic(&FDHMainMenuBar::OnSelectIDE,EIDEType::VSCode),
                           FCanExecuteAction(),
                           FIsActionChecked::CreateStatic( &FDHMainMenuBar::OnCheckIDE,EIDEType::VSCode));

    CommandList->MapAction(FMainMenuBarCommands::Get().SelectIDEA,
                           FExecuteAction::CreateStatic(&FDHMainMenuBar::OnSelectIDE, EIDEType::IDEA),
                           FCanExecuteAction(),
                           FIsActionChecked::CreateStatic(&FDHMainMenuBar::OnCheckIDE, EIDEType::IDEA));
}

FDHMainMenuBar::~FDHMainMenuBar()
{
    FMainMenuBarCommands::Unregister();

}

void FDHMainMenuBar::Init()
{
    Extender = MakeShareable(new FExtender);
    Extender->AddToolBarExtension("Settings", EExtensionHook::After, CommandList,
                                  FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& Builder)
                                  {
                                      Builder.BeginSection(NAME_None);
                                      Builder.AddComboButton(FUIAction(),
                                                             FOnGetContent::CreateRaw(this, &FDHMainMenuBar::GenerateMenu),
                                                             LOCTEXT("UnLuaDevHelper_Label", "UnLuaDevHelper"),
                                                             LOCTEXT("UnLuaDevHelper_ToolTip", "UnLuaDevHelper"),
                                                             FSlateIcon("UnluaDevHelperStyle", "UnluaDevHelperEditor.Logo")
                                      );
                                      Builder.EndSection();
                                  })
    );
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(Extender);
}

void FDHMainMenuBar::Clear() const
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(Extender);
}


TSharedRef<SWidget> FDHMainMenuBar::GenerateMenu()
{
    const FMainMenuBarCommands& Commands = FMainMenuBarCommands::Get();

    FMenuBuilder MenuBuilder(true, CommandList);

    MenuBuilder.BeginSection(NAME_None, LOCTEXT("Section_Action", "Action"));
    MenuBuilder.AddMenuEntry(Commands.OpenSoulation, NAME_None, LOCTEXT("OpenSoulation", "Open Soulation"));
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection(NAME_None, LOCTEXT("Section_Setting", "Setting"));
    MenuBuilder.AddMenuEntry(Commands.EnableDebug, NAME_None, LOCTEXT("EnableDebug", "Enable Debug"));
    MenuBuilder.AddSubMenu(LOCTEXT("Section_ChangeIDE", "ChangeIDE"),
                           LOCTEXT("Section_ChangeIDE_ToolTip", "Change Lua IDE"),
                           FNewMenuDelegate::CreateStatic(&FDHMainMenuBar::MakeIDEChangeMenu));
    MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
}


void FDHMainMenuBar::OnOpenSoulation()
{
    FUnluaDevHelperEditorModule& UnluaDevHelperEditor=FModuleManager::Get().LoadModuleChecked<FUnluaDevHelperEditorModule>(TEXT("UnluaDevHelperEditor"));
    UnluaDevHelperEditor.OpenSolution();
}

void FDHMainMenuBar::OnSelectIDE(EIDEType Type)
{
    FUnluaDevHelperEditorModule& UnluaDevHelperEditor=FModuleManager::Get().LoadModuleChecked<FUnluaDevHelperEditorModule>(TEXT("UnluaDevHelperEditor"));
    UnluaDevHelperEditor.ChangeIDE(Type);
}

bool FDHMainMenuBar::OnCheckIDE(EIDEType Type)
{
    EIDEType IDEType;
    FRegistryManager::Get().GetEnum(EDevHelperSettingToString(EDevHelperSetting::IDEType),IDEType);
    return IDEType==Type;
}


void FDHMainMenuBar::OnEnableDebug()
{
    bool bEnableDebug=false;
    FRegistryManager::Get().GetBool(EDevHelperSettingToString(EDevHelperSetting::bEnableDebug),bEnableDebug);
    FRegistryManager::Get().SetBool(EDevHelperSettingToString(EDevHelperSetting::bEnableDebug),!bEnableDebug);
}

bool FDHMainMenuBar::IsEnableDebugCheck()
{
    bool bEnableDebug=false;
    FRegistryManager::Get().GetBool(EDevHelperSettingToString(EDevHelperSetting::bEnableDebug),bEnableDebug);
    return bEnableDebug;
}


void FDHMainMenuBar::MakeIDEChangeMenu(FMenuBuilder& MenuBuilde)
{
    {
        MenuBuilde.BeginSection(NAME_None, LOCTEXT("Section_ChangeIDE", "ChangeIDE"));
        MenuBuilde.AddMenuEntry(FMainMenuBarCommands::Get().SelectVSCode, NAME_None, LOCTEXT("SelectVSCode", "VSCode"));
        MenuBuilde.AddMenuEntry(FMainMenuBarCommands::Get().SelectIDEA, NAME_None, LOCTEXT("SelectIDEA", "IDEA"));
        MenuBuilde.EndSection();
    }
}


#undef LOCTEXT_NAMESPACE 
