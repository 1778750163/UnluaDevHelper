// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FSyntaxHighlighterTextLayoutMarshaller;
/**
 * 
 */
class UNLUADEVHELPEREDITOR_API SLuaFileInfo : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SLuaFileInfo)
        {
        }
        SLATE_ATTRIBUTE(FString,BlueprintPath)
    SLATE_END_ARGS()
    
    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

private:

    FReply OnCreateLuaFileButtonClicked();

    bool ShowOverwriteWarningDialog(const FString& InFilePath);
    
    void OnComboxSelect(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

    void OnTextChanged(const FText& Text);

    FText OnBlueprintPath() const;
    FText OnFilePath() const;
    
    void RefreshFilePath();

    void RefreshFileText();

    FString GetTemplateFilePath();

    FText GetRichEditableText() const;

    void HandleRichEditableTextChanged(const FText& Text);

    void HandleRichEditableTextCommitted(const FText& Text, ETextCommit::Type Type);

    ECheckBoxState IsEnableSyntaxHighlightingChecked() const;

    void OnEnableSyntaxHighlightingChanged(ECheckBoxState InState);

private:

    FString BlueprintPath;
    FString BlueprintName;
    
    FText RichEditableText;
    TSharedPtr<FSyntaxHighlighterTextLayoutMarshaller> SyntaxHighlighterMarshaller;
    
    TArray< TSharedPtr<FString> > SpeedSettingStrings;
    TSharedPtr<STextComboBox>   TextComboBox;
    FString CheckLuaFileStr;
    FString LuaFileName;
    FString FilePath;
    FString SubPath;
    FString FileSuffx;
};
