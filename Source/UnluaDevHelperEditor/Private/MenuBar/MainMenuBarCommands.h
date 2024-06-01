
#pragma once

#include "Framework/Commands/Commands.h"

class FMainMenuBarCommands : public TCommands<FMainMenuBarCommands>
{
public:
    FMainMenuBarCommands()
        : TCommands<FMainMenuBarCommands>(
            TEXT("UnluaDevHelperEditor"), NSLOCTEXT("Contexts", "UnluaDevHelperEditor", "Unlua Dev Helper Editor"),
            NAME_None, "UnluaDevHelperStyle")
    {
    }

    virtual void RegisterCommands() override;

    TSharedPtr<FUICommandInfo> OpenSoulation;
    TSharedPtr<FUICommandInfo> SelectVSCode;
    TSharedPtr<FUICommandInfo> SelectIDEA;
    TSharedPtr<FUICommandInfo> EnableDebug;
    TSharedPtr<FUICommandInfo> ChoosePath;
};
