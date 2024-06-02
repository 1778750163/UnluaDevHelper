#include "DHBlueprinBar.h"

#include "BlueprintEditor.h"
#include "DHBlueprintBarCommands.h"
#include "BlueprintEditorModule.h"
#include "UnluaDevHelperDefine.h"
#include "UnluaDevHelperEditor.h"
#include "UnLuaInterface.h"
#include "CreateLua/LuaFileInfo.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Toolkits/AssetEditorManager.h"


#define LOCTEXT_NAMESPACE "FDHBlueprinBar" 

const FName FDHBlueprinBar::CreateLuaFileTabName("CreateLuaFile");


FDHBlueprinBar::FDHBlueprinBar()
    : CommandList(new FUICommandList),
      Blueprint(nullptr)
{
    FDHBlueprintBarCommands::Register();

    CommandList->MapAction(FDHBlueprintBarCommands::Get().BindLua,FExecuteAction::CreateRaw(this,&FDHBlueprinBar::OnBindLua));
    CommandList->MapAction(FDHBlueprintBarCommands::Get().UnBindLua,FExecuteAction::CreateRaw(this,&FDHBlueprinBar::OnBindLua));
    CommandList->MapAction(FDHBlueprintBarCommands::Get().OpenLua,FExecuteAction::CreateRaw(this,&FDHBlueprinBar::OnOpenLua));
    CommandList->MapAction(FDHBlueprintBarCommands::Get().CreateLuaFile,FExecuteAction::CreateRaw(this,&FDHBlueprinBar::OnCreateLuaFile));
    CommandList->MapAction(FDHBlueprintBarCommands::Get().CopyRelativePath,FExecuteAction::CreateRaw(this,&FDHBlueprinBar::OnCopyRelativePath));
}

FDHBlueprinBar::~FDHBlueprinBar()
{
    
}

void FDHBlueprinBar::Init()
{

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 23
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().AddLambda([this](UObject* Boj,IAssetEditorInstance* AssetEditorInstance)
	{
		Blueprint=Cast<UBlueprint>(Boj);
	});
#else
	FAssetEditorManager::Get().OnAssetOpenedInEditor().AddLambda([this](UObject* Boj,IAssetEditorInstance* AssetEditorInstance)
	{
		Blueprint=Cast<UBlueprint>(Boj);
	});
#endif

    
    FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
    auto& ExtenderDelegates = BlueprintEditorModule.GetMenuExtensibilityManager()->GetExtenderDelegates();
    ExtenderDelegates.Add(FAssetEditorExtender::CreateRaw(this,&FDHBlueprinBar::GenerateBlueprinBar));

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CreateLuaFileTabName, FOnSpawnTab::CreateRaw(this, &FDHBlueprinBar::OnSpawnCreateLuaFileTab))
    .SetDisplayName(LOCTEXT("FCreateLuaFileTabTitle", "CreateLuaFile"))
    .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FDHBlueprinBar::Clear()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CreateLuaFileTabName);
}

TSharedRef<FExtender> FDHBlueprinBar::GenerateBlueprinBar(const TSharedRef<FUICommandList> InCommandList, const TArray<UObject*> Objects) const
{
    TSharedRef<FExtender> Extender=MakeShareable(new FExtender);
    if(Objects.Num()==0) return Extender;
    UObject* ContentObj=Objects[0];
    if(!IsValid(ContentObj)) return Extender;
    auto const& ToolBarExtensionDelegate=FToolBarExtensionDelegate::CreateLambda([&,this,ContentObj ](FToolBarBuilder& ToolbarBuilder)
                                  {
                                      ConstructMenu(ToolbarBuilder,ContentObj);
                                  });
    Extender->AddToolBarExtension("Debugging", EExtensionHook::After, CommandList,ToolBarExtensionDelegate);

    return Extender;
}

void FDHBlueprinBar::ConstructMenu(FToolBarBuilder& ToolbarBuilder,UObject* Obj) const 
{
    auto const& BlueprintTemp=Cast<UBlueprint>(Obj);
    if(!IsValid(BlueprintTemp)) return;
    ToolbarBuilder.BeginSection(NAME_None);

    EBlueprintBindState BlueprintBindState=GetBlueprintBindState(BlueprintTemp);
    FSlateIcon SlateIcon("UnluaDevHelperStyle","UnluaDevHelperEditor.Logo");
    switch (BlueprintBindState)
    {
    case EBlueprintBindState::Unknown:
        {
            SlateIcon=FSlateIcon("UnluaDevHelperStyle","UnluaDevHelperEditor.LogoWarning");
        }
        break;
    case EBlueprintBindState::BindWithoutCreatingFile:
        {
            SlateIcon=FSlateIcon("UnluaDevHelperStyle","UnluaDevHelperEditor.LogoError");
        }
        break;
    case EBlueprintBindState::BindSucceed:
        {
            SlateIcon=FSlateIcon("UnluaDevHelperStyle","UnluaDevHelperEditor.LogoSucceed");
        }
        break;
    }
    ToolbarBuilder.AddComboButton(FUIAction(), FOnGetContent::CreateLambda([&,Obj, this]()->TSharedRef<SWidget>
        {
            Blueprint=Cast<UBlueprint>(Obj);
            return ConstructSubMenu();
        }),
        LOCTEXT("UnluaHelper","Unlua Helper"),
        LOCTEXT("UnluaHelperTip","Unlua Helper Tip"),
        SlateIcon
        );
    ToolbarBuilder.EndSection();
}

TSharedRef<SWidget> FDHBlueprinBar::ConstructSubMenu() const
{
    const FDHBlueprintBarCommands& Commands = FDHBlueprintBarCommands::Get();
    FMenuBuilder MenuBuilder(true, CommandList->AsShared());
    EBlueprintBindState BlueprintBindState=GetBlueprintBindState(Blueprint);
    switch (BlueprintBindState) {
    case EBlueprintBindState::Unknown:
    case EBlueprintBindState::UnBind:
        {
            MenuBuilder.AddMenuEntry(Commands.BindLua,NAME_None,LOCTEXT("BindLua", "Bind Lua"));
        }
        break;
    case EBlueprintBindState::BindWithoutCreatingFile:
        {
            MenuBuilder.AddMenuEntry(Commands.CreateLuaFile,NAME_None,LOCTEXT("CreateLuaFile", "Create Lua File"));
            MenuBuilder.AddMenuEntry(Commands.UnBindLua,NAME_None,LOCTEXT("UnBindLua", "UnBind Lua"));
        }
        break;
    case EBlueprintBindState::BindSucceed:
        {
            MenuBuilder.AddMenuEntry(Commands.OpenLua,NAME_None,LOCTEXT("OpenLua", "Open Lua"));
            MenuBuilder.AddMenuEntry(Commands.CreateLuaFile,NAME_None,LOCTEXT("CreateLuaFile", "Create Lua File"));
            MenuBuilder.AddMenuEntry(Commands.CopyRelativePath,NAME_None,LOCTEXT("CopyRelativePath", "Copy Relative Path"));
            MenuBuilder.AddMenuEntry(Commands.UnBindLua,NAME_None,LOCTEXT("UnBindLua", "UnBind Lua"));
        }
        break;
    }
    return MenuBuilder.MakeWidget();
}

void FDHBlueprinBar::OnBindLua() const
{
    if (!IsValid(Blueprint))
        return;

    const auto TargetClass = Blueprint->GeneratedClass;
    if (!IsValid(TargetClass))
        return;

    if (TargetClass->ImplementsInterface(UUnLuaInterface::StaticClass()))
        return;

    
    const auto Ok = FBlueprintEditorUtils::ImplementNewInterface(Blueprint, FName("UnLuaInterface"));
    if(!Ok) return;
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 23    
    const auto BlueprintEditors = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet").GetBlueprintEditors();
	for (auto BlueprintEditor : BlueprintEditors)
	{
		const auto MyBlueprintEditor = static_cast<FBlueprintEditor*>(&BlueprintEditors[0].Get());
		if (!MyBlueprintEditor || MyBlueprintEditor->GetBlueprintObj() != Blueprint)
			continue;
		MyBlueprintEditor->Compile();
	}
#endif
}

void FDHBlueprinBar::OnUnBindLua() const
{
    if (!IsValid(Blueprint))
        return;

    const auto TargetClass = Blueprint->GeneratedClass;
    if (!IsValid(TargetClass))
        return;

    if (!TargetClass->ImplementsInterface(UUnLuaInterface::StaticClass()))
        return;
    
    FBlueprintEditorUtils::RemoveInterface(Blueprint, FName("UnLuaInterface"));
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 23 
    const auto BlueprintEditors = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet").GetBlueprintEditors();
    for (auto BlueprintEditor : BlueprintEditors)
    {
        const auto MyBlueprintEditor = static_cast<FBlueprintEditor*>(&BlueprintEditors[0].Get());
        if (!MyBlueprintEditor || MyBlueprintEditor->GetBlueprintObj() != Blueprint)
            continue;
        MyBlueprintEditor->Compile();
    }
#endif
}

void FDHBlueprinBar::OnCreateLuaFile() 
{
    FGlobalTabmanager::Get()->TryInvokeTab(CreateLuaFileTabName);
}

TSharedRef<class SDockTab> FDHBlueprinBar::OnSpawnCreateLuaFileTab(const class FSpawnTabArgs& SpawnTabArgs)
{
    FString BlueprintPath="";
    if(IsValid(Blueprint))
    {
        BlueprintPath=Blueprint->GetPathName();
    }
    
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            // Put your tab content here!
            SNew(SBox)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SLuaFileInfo)
                .BlueprintPath(BlueprintPath)
            ]
        ];
}

void FDHBlueprinBar::OnCopyRelativePath() const
{
}

void FDHBlueprinBar::OnOpenLua() const
{
    if (!IsValid(Blueprint))
        return;

    const auto InterfaceDesc = *Blueprint->ImplementedInterfaces.FindByPredicate([](const FBPInterfaceDescription& Desc)
    {
        return Desc.Interface == UUnLuaInterface::StaticClass();
    });
    FString Path=InterfaceDesc.Graphs[0]->Nodes[1]->Pins[1]->DefaultValue ;

    FUnluaDevHelperEditorModule& UnluaDevHelperEditorModule=FModuleManager::LoadModuleChecked<FUnluaDevHelperEditorModule>(TEXT("UnluaDevHelperEditor"));
    UnluaDevHelperEditorModule.OpenSolution(FString::Printf(TEXT("%s.lua"),*Path));
}


#undef LOCTEXT_NAMESPACE