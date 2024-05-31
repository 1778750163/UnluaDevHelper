#pragma once

#include "CoreMinimal.h"

class FExtender;
class FUICommandList;
enum class EIDEType : uint8;


class FMenuBuilder;
class SWidget;

class FDHMainMenuBar
{
public:
    FDHMainMenuBar();
    ~FDHMainMenuBar();
    
    void Init();
    void Clear() const;

private:

    TSharedRef<SWidget> GenerateMenu();


    void OnOpenSoulation();

    static void OnSelectIDE(EIDEType Type);
    static bool OnCheckIDE(EIDEType Type);

    static void OnEnableDebug();
    static bool IsEnableDebugCheck();

    
    static void MakeIDEChangeMenu(FMenuBuilder& MenuBuilde);
    
private:
    const TSharedRef<FUICommandList> CommandList;
    TSharedPtr<FExtender> Extender;
};



