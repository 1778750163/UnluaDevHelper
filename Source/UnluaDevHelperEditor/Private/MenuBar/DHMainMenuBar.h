#pragma once

#include "CoreMinimal.h"

enum class EIdeaDebugMode : uint8;
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

    void OnSelectIDE(EIDEType Type);
    bool OnCheckIDE(EIDEType Type);

    void OnEnableDebug();
    bool IsEnableDebugCheck();
    
    void MakeIDEChangeMenu(FMenuBuilder& MenuBuilde);

    void MakeDataSetting(FMenuBuilder& MenuBuilde);

    void MakeIDEDebugModeMenu(FMenuBuilder& MenuBuilde);
    void OnSelectIdeaDebugMode(EIdeaDebugMode Type);
    bool OnCheckIdeaDebugMode(EIdeaDebugMode Type);
    
    FText OnUserName();
    void OnUserNameTextCommitted(const FText& Text, ETextCommit::Type Type);

    FText OnIdeaName();
    void OnIdeaNameTextCommitted(const FText& Text, ETextCommit::Type Type);
    
private:
    const TSharedRef<FUICommandList> CommandList;
    TSharedPtr<FExtender> Extender;
    FString SelectedPath;
    TSharedPtr<STextBlock> PathDisplayTextBlock;
    FString  UserName;
    FString  IdeaName;
};



