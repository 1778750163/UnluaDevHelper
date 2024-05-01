#pragma once

class FDHBlueprinBar
{
public:
    FDHBlueprinBar();

    ~FDHBlueprinBar();

    void Init();

    void Clear();


    TSharedRef<FExtender> GenerateBlueprinBar(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> Objects) const;

    void ConstructMenu(FToolBarBuilder& ToolbarBuilder,UObject* Obj) const;

    TSharedRef<SWidget> ConstructSubMenu() const;

private:
    void OnBindLua() const;

    void OnUnBindLua() const;

    void OnCreateLuaFile() const;

    void OnCopyRelativePath() const;
    
    void OnOpenLua() const;

private:
    const TSharedRef<FUICommandList> CommandList;

    mutable UBlueprint* Blueprint;
};
