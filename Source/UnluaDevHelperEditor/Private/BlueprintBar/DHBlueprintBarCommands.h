#pragma once

class FDHBlueprintBarCommands : public TCommands<FDHBlueprintBarCommands>
{
public:
    FDHBlueprintBarCommands()
        : TCommands<FDHBlueprintBarCommands>(
            TEXT("DHBlueprintBarCommands"), NSLOCTEXT("Contexts", "DHBlueprintBarCommands", "Unlua Dev Helper Editor"),
            NAME_None, "UnluaDevHelperStyle")
    {
    }

    virtual void RegisterCommands() override;

    TSharedPtr<FUICommandInfo> BindLua;
    TSharedPtr<FUICommandInfo> UnBindLua;
    TSharedPtr<FUICommandInfo> OpenLua;
    TSharedPtr<FUICommandInfo> CreateLuaFile;
    TSharedPtr<FUICommandInfo> CopyRelativePath;
};
