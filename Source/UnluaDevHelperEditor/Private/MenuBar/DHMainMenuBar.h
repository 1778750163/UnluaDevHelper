#pragma once

enum class EIDEType : uint8;

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

    
    static void MakeIDEChangeMenu(FMenuBuilder&);
    
private:
    const TSharedRef<FUICommandList> CommandList;
    TSharedPtr<FExtender> Extender;
};



