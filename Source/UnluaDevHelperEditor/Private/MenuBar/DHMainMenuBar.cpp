#include "MenuBar/DHMainMenuBar.h"

#include "LevelEditor.h"
#include "MainMenuBarCommands.h"
#include "ToolMenu.h"
#include "ToolMenuDelegates.h"
#include "ToolMenus.h"
#include "UnluaDevHelperDefine.h"
#include "UnluaDevHelperEditor.h"
#include "UnluaDevHelperSetting.h"

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
    const auto& Settings = *GetDefault<UUnluaDevHelperSetting>();
    return Settings.IDEType==Type;
}

void FDHMainMenuBar::OnEnableDebug()
{
    auto& Settings = *GetMutableDefault<UUnluaDevHelperSetting>();
    Settings.bEnableDebug=!Settings.bEnableDebug;
    Settings.UpdateProjectUserConfigFile();
}

bool FDHMainMenuBar::IsEnableDebugCheck()
{
    const auto& Settings = *GetDefault<UUnluaDevHelperSetting>();
    return Settings.bEnableDebug;
}


void FDHMainMenuBar::MakeIDEChangeMenu(FMenuBuilder& MenuBuilde)
{
    {
        MenuBuilde.BeginSection(NAME_None, LOCTEXT("Section_ChangeIDE", "IDE"));
        MenuBuilde.AddMenuEntry(FMainMenuBarCommands::Get().SelectVSCode, NAME_None, LOCTEXT("SelectVSCode", "VSCode"));
        MenuBuilde.AddMenuEntry(FMainMenuBarCommands::Get().SelectIDEA, NAME_None, LOCTEXT("SelectIDEA", "IDEA"));
        MenuBuilde.EndSection();
    }
}


#undef LOCTEXT_NAMESPACE 
