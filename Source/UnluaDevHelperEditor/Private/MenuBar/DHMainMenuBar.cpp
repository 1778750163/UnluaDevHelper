

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
                           FExecuteAction::CreateRaw(this,&FDHMainMenuBar::OnEnableDebug),
                           FCanExecuteAction(),
                           FIsActionChecked::CreateRaw(this,&FDHMainMenuBar::IsEnableDebugCheck));
    
    CommandList->MapAction(FMainMenuBarCommands::Get().SelectVSCode,
                           FExecuteAction::CreateRaw(this,&FDHMainMenuBar::OnSelectIDE,EIDEType::VSCode),
                           FCanExecuteAction(),
                           FIsActionChecked::CreateRaw(this, &FDHMainMenuBar::OnCheckIDE,EIDEType::VSCode));

    CommandList->MapAction(FMainMenuBarCommands::Get().SelectIDEA,
                           FExecuteAction::CreateRaw(this,&FDHMainMenuBar::OnSelectIDE, EIDEType::IDEA),
                           FCanExecuteAction(),
                           FIsActionChecked::CreateRaw(this,&FDHMainMenuBar::OnCheckIDE, EIDEType::IDEA));

    CommandList->MapAction(FMainMenuBarCommands::Get().TcpConnect,
                       FExecuteAction::CreateRaw(this,&FDHMainMenuBar::OnSelectIdeaDebugMode,EIdeaDebugMode::TcpConnect),
                       FCanExecuteAction(),
                       FIsActionChecked::CreateRaw(this, &FDHMainMenuBar::OnCheckIdeaDebugMode,EIdeaDebugMode::TcpConnect));
    CommandList->MapAction(FMainMenuBarCommands::Get().TcpListen,
                   FExecuteAction::CreateRaw(this,&FDHMainMenuBar::OnSelectIdeaDebugMode,EIdeaDebugMode::TcpListen),
                   FCanExecuteAction(),
                   FIsActionChecked::CreateRaw(this, &FDHMainMenuBar::OnCheckIdeaDebugMode,EIdeaDebugMode::TcpListen));
}

FDHMainMenuBar::~FDHMainMenuBar()
{
    FMainMenuBarCommands::Unregister();

}

void FDHMainMenuBar::Init()
{
    FRegistryManager::Get().GetString(EDevHelperSettingToString(EDevHelperSetting::UserName),UserName);
    FRegistryManager::Get().GetString(EDevHelperSettingToString(EDevHelperSetting::IdeaName),IdeaName);
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
                           FNewMenuDelegate::CreateRaw(this,&FDHMainMenuBar::MakeIDEChangeMenu));
    MenuBuilder.AddSubMenu(LOCTEXT("Section_IdeaDebugMode", "IdeaDebugMode"),
               LOCTEXT("Section_IdeaDebugMode_ToolTip", "Change Idea Debug Mode"),
               FNewMenuDelegate::CreateRaw(this,&FDHMainMenuBar::MakeIDEDebugModeMenu));
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection(NAME_None, LOCTEXT("Section_DataSetting", "DataSetting"));
    MenuBuilder.AddSubMenu(LOCTEXT("Section_DataSetting", "DataSetting"),
                       LOCTEXT("Section_DataSetting_ToolTip", "DataSetting"),
                       FNewMenuDelegate::CreateRaw(this,&FDHMainMenuBar::MakeDataSetting));
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

void FDHMainMenuBar::MakeDataSetting(FMenuBuilder& MenuBuilde)
{
    {
        MenuBuilde.BeginSection(NAME_None, LOCTEXT("Section_Config", "Config"));
        MenuBuilde.AddEditableText(LOCTEXT("Section_UserName", "UserName"),LOCTEXT("Section_UserNameTip","input your name"),FSlateIcon(),
        TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this,&FDHMainMenuBar::OnUserName)),
            FOnTextCommitted::CreateRaw(this,&FDHMainMenuBar::OnUserNameTextCommitted));
        
        MenuBuilde.AddEditableText(LOCTEXT("Section_IdeaName", "IdeaName"),
            LOCTEXT("Section_DebugConfigTip","input str \" package.cpath = package.cpath .. ';C:/Users/17787/AppData/Roaming/JetBrains/IntelliJIdea2024.1/plugins/EmmyLua/debugger/emmy/windows/x64/?.dll'\"  \n Substring \n IntelliJIdea2024.1"),
            FSlateIcon(),
        TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this,&FDHMainMenuBar::OnIdeaName)),
            FOnTextCommitted::CreateRaw(this,&FDHMainMenuBar::OnIdeaNameTextCommitted));
        MenuBuilde.EndSection();
    }
}

void FDHMainMenuBar::MakeIDEDebugModeMenu(FMenuBuilder& MenuBuilde)
{
    {
        MenuBuilde.BeginSection(NAME_None, LOCTEXT("Section_IdeaDebugMode", "IdeaDebugMode"));
        MenuBuilde.AddMenuEntry(FMainMenuBarCommands::Get().TcpListen, NAME_None, LOCTEXT("TcpListen", "TcpListen"));
        MenuBuilde.AddMenuEntry(FMainMenuBarCommands::Get().TcpConnect, NAME_None, LOCTEXT("TcpConnect", "TcpConnect"));
        MenuBuilde.EndSection();
    }
}

void FDHMainMenuBar::OnSelectIdeaDebugMode(EIdeaDebugMode Type)
{
    FUnluaDevHelperEditorModule& UnluaDevHelperEditor=FModuleManager::Get().LoadModuleChecked<FUnluaDevHelperEditorModule>(TEXT("UnluaDevHelperEditor"));
    UnluaDevHelperEditor.ChangeIdeaDebugMode(Type);
}

bool FDHMainMenuBar::OnCheckIdeaDebugMode(EIdeaDebugMode Type)
{
    EIdeaDebugMode IDEType;
    FRegistryManager::Get().GetEnum(EDevHelperSettingToString(EDevHelperSetting::IdeaDebugMode),IDEType);
    return IDEType==Type;
}

FText FDHMainMenuBar::OnUserName()
{
    return FText::FromString(UserName);
}

void FDHMainMenuBar::OnUserNameTextCommitted(const FText& Text, ETextCommit::Type Type) 
{
    UserName=Text.ToString();
    FRegistryManager::Get().SetString(EDevHelperSettingToString(EDevHelperSetting::UserName),UserName);
}

FText FDHMainMenuBar::OnIdeaName()
{
    return FText::FromString(IdeaName);
}

void FDHMainMenuBar::OnIdeaNameTextCommitted(const FText& Text, ETextCommit::Type Type) 
{
    IdeaName=Text.ToString();
    FRegistryManager::Get().SetString(EDevHelperSettingToString(EDevHelperSetting::IdeaName),IdeaName);
}


#undef LOCTEXT_NAMESPACE 
