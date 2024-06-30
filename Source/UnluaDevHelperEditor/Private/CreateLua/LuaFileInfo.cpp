// Fill out your copyright notice in the Description page of Project Settings.


#include "LuaFileInfo.h"

#include "CustomNotification.h"
#include "EditorStyleSet.h"
#include "LuaRichTextSyntaxHighlighterTextLayoutMarshaller.h"

#include "SlateOptMacros.h"
#include "TestStyle.h"
#include "UnluaDevHelperEditor.h"
#include "UnluaDevHelperSetting.h"
#include "UnluaDevHelperStyle.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Misc/FileHelper.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 23
#include "SEditableTextBox.h"
#endif


#define LOCTEXT_NAMESPACE "SLuaFileInfo"

class FUnluaDevHelperEditorModule;
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLuaFileInfo::Construct(const FArguments& InArgs)
{
	BlueprintPath=InArgs._BlueprintPath.Get();
	{
	    int32 LastSlashIndex;
	    if(BlueprintPath.FindLastChar('.', LastSlashIndex))
	    {
	        BlueprintName=BlueprintPath.Mid(LastSlashIndex+1);
	    }
	}
    
    
	SpeedSettingStrings.Empty();
	RefreshFilePath();
	CheckLuaFileStr="";
	LuaFileName="";

	SyntaxHighlighterMarshaller = FLuaSyntaxHighlighterTextLayoutMarshaller::Create(
		FLuaSyntaxHighlighterTextLayoutMarshaller::FSyntaxTextStyle()
		);
	
	UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
	if(IsValid(Setting))
	{
		for (auto LuaInfo:Setting->LuaFileCreationRules)
		{
			if(LuaInfo.Key.Len())
			{
				SpeedSettingStrings.Add(MakeShareable( new FString(LuaInfo.Key) ));
			}
		}
	}
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		.Padding(10.0f, 10.0f, 10.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.MaxWidth(150)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("BlueprintPath", "Blueprint Path"))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SEditableTextBox)
				.Text(this, &SLuaFileInfo::OnBlueprintPath)
				.IsReadOnly(true)
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		.Padding(10.0f, 10.0f, 10.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.MaxWidth(150)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SpawnLuaFilePath", "Spawn Lua File Path"))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SEditableTextBox)
				.Text(this, &SLuaFileInfo::OnFilePath)
				.IsReadOnly(true)
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.Padding(10.0f, 10.0f, 10.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(150)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.Content()
				[
					SAssignNew(TextComboBox,STextComboBox)
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Light")
					.OptionsSource(&SpeedSettingStrings)
					.OnSelectionChanged(this, &SLuaFileInfo::OnComboxSelect)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(20.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LuaFileName", "Lua File Name : "))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Content()
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("InputLuaFileName", "Input Lua File Name"))
					.OnTextChanged(this,&SLuaFileInfo::OnTextChanged)
				]
			]
		]
		// Source text editor
		+ SVerticalBox::Slot()
		.FillHeight(1000)
		.Padding(10.0f, 10.0f, 10.0f, 0.0f)
		[
			SNew(SBorder)
			.Padding(5.0f)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.BorderImage(FCoreStyle::Get().GetBrush("BoxShadow"))
			[
				SNew(SBorder)
				.BorderImage(FTestStyle::Get().GetBrush("RichText.Interactive.Details.Background"))
				.Padding(FMargin(4))
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
					[
						SNew(SMultiLineEditableTextBox)
						.BackgroundColor(FLinearColor(FColor(0xff282c34)))
						.ForegroundColor(FLinearColor(FColor(0xffabb2bf)))
						.Text(this, &SLuaFileInfo::GetRichEditableText)
						.OnTextChanged(this, &SLuaFileInfo::HandleRichEditableTextChanged)
						.OnTextCommitted(this, &SLuaFileInfo::HandleRichEditableTextCommitted)
						.Marshaller(SyntaxHighlighterMarshaller)
						.AutoWrapText(false)
						.Margin(4)
						.LineHeightPercentage(1.1f)
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBorder)
						//.BorderImage(FTestStyle::Get().GetBrush("RichText.RoundedBackground"))
						.Padding(FMargin(4))
						.HAlign(HAlign_Right)
						[
							SNew(SCheckBox)
							.Style(FTestStyle::Get(), "RichText.Toolbar.Checkbox")
							.IsChecked(this, &SLuaFileInfo::IsEnableSyntaxHighlightingChecked)
							.OnCheckStateChanged(this, &SLuaFileInfo::OnEnableSyntaxHighlightingChanged)
							[
								SNew(STextBlock)
								.TextStyle(FTestStyle::Get(), "RichText.Toolbar.Text")
								.Text(LOCTEXT("EnableSyntaxHighlightingLabel", "Enable Syntax Highlighting"))
							]
						]
					]
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.Padding(10.0f, 10.0f, 10.0f, 0.0f)
		[
			SNew(SBox)
			.WidthOverride(320)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.Content()
			[
				SNew(SButton)
				.Text(LOCTEXT("CreateLuaFile", "Create Lua File"))
				.OnClicked(this,&SLuaFileInfo::OnCreateLuaFileButtonClicked)
			]
		]
	];
	if(TextComboBox.IsValid()&&SpeedSettingStrings.Num())
	{
		TextComboBox->SetSelectedItem(SpeedSettingStrings[0]);
	}
}

FReply SLuaFileInfo::OnCreateLuaFileButtonClicked()
{
    FString TemplateFilePath=GetTemplateFilePath();
    if(TemplateFilePath.Len()==0) return FReply::Handled();
    FString NewFilePath=FilePath;
    NewFilePath.ReplaceInline(TEXT("."),TEXT("/"));
    NewFilePath+=".lua";
    FString FullPath = FPaths::ConvertRelativePathToFull( FPaths::Combine(GetLuaProjectPath(),NewFilePath));
    bool bShouldOverwrite=true;
    if(FPaths::FileExists(FullPath))
    {
        // 文件已存在，显示警示弹窗
        bShouldOverwrite = ShowOverwriteWarningDialog(FullPath);
    }
    if(bShouldOverwrite)
    {
        if (FFileHelper::SaveStringToFile(RichEditableText.ToString(), *FullPath))
        {
            UCustomNotification::ShowLinkNotification(FText::FromString(TEXT("Create File Succeed!")),FText::FromString(FullPath),FSimpleDelegate::CreateLambda([NewFilePath]()
            {
                FUnluaDevHelperEditorModule& UnluaDevHelperEditor=FModuleManager::Get().LoadModuleChecked<FUnluaDevHelperEditorModule>(TEXT("UnluaDevHelperEditor"));
                UnluaDevHelperEditor.OpenSolution(NewFilePath);
            }));
        }
    }
	return FReply::Handled();
}

bool SLuaFileInfo::ShowOverwriteWarningDialog(const FString& InFilePath)
{
    FText Title = FText::FromString(TEXT("File Exists"));
    FText Message = FText::Format(FText::FromString(TEXT("The file '{0}' already exists. Do you want to overwrite it?")), FText::FromString(InFilePath));

    EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, Message, &Title);

    return ReturnType == EAppReturnType::Yes;
}

void SLuaFileInfo::OnComboxSelect(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	CheckLuaFileStr=NewSelection.ToSharedRef().Get();
	RefreshFilePath();
}

void SLuaFileInfo::OnTextChanged(const FText& Text)
{
	LuaFileName=Text.ToString();
	RefreshFilePath();
}

FText SLuaFileInfo::OnBlueprintPath() const
{
	return FText::FromString(BlueprintPath);
}

FText SLuaFileInfo::OnFilePath() const
{
	return FText::FromString(FilePath);
}

void SLuaFileInfo::RefreshFilePath()
{
    FString BPPath=BlueprintPath;
    {
        int32 LastSlashIndex;
        if (BPPath.FindLastChar('/', LastSlashIndex))
        {
            BPPath=BPPath.Left(LastSlashIndex);
        }
        LastSlashIndex=BPPath.Find(TEXT("/Game/"),ESearchCase::CaseSensitive);
        if(LastSlashIndex>=0)
        {
            BPPath=BPPath.Mid(LastSlashIndex+6);
        }
    }
	FString Path=BPPath;
	SubPath="";
	FileSuffx="";
	UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
	if(IsValid(Setting))
	{
		for (auto LuaInfo:Setting->LuaFileCreationRules)
		{
			if(LuaInfo.Key.Len() && LuaInfo.Key==CheckLuaFileStr)
			{
				SubPath=LuaInfo.Value.SubDirectory;
				FileSuffx=LuaInfo.Value.FileSuffix;
			    if(LuaInfo.Value.bIsFixedGenerationPath && LuaInfo.Value.FixedGenerationPath.Len())
			    {
			        Path=LuaInfo.Value.FixedGenerationPath; 
			    }
				break;
			}
		}
	}

	Path = SubPath.Len() > 0 ? FPaths::Combine(Path, SubPath) : Path;
    
	if(LuaFileName.Len())
	{
		Path=FPaths::Combine(Path,LuaFileName)+FileSuffx;
	}
    else
    {
        Path=FPaths::Combine(Path,BlueprintName)+FileSuffx;
    }

    
    FilePath=Path;

    
	FilePath.ReplaceInline(TEXT("\\\\"),TEXT("."));
	FilePath.ReplaceInline(TEXT("//"),TEXT("."));
	FilePath.ReplaceInline(TEXT("/"),TEXT("."));
	FilePath.ReplaceInline(TEXT("\\"),TEXT("."));
    RefreshFileText();
}

void SLuaFileInfo::RefreshFileText()
{
    FString TemplateFilePath=GetTemplateFilePath();
    FString Result;
    if (FFileHelper::LoadFileToString(Result, *TemplateFilePath))
    {
        // BlueprintPath
        {
            int32 Index=-1;
            if(BlueprintPath.FindLastChar('.',Index))
            {
                Result=Result.Replace(TEXT("@{BlueprintPath}"),*BlueprintPath.Left(Index),ESearchCase::Type::CaseSensitive);
            }
        }
        // LuaFilePath
        {
           Result = Result.Replace(TEXT("@{LuaFilePath}"),*FilePath,ESearchCase::Type::CaseSensitive);
        }
        // UserName
        {
            FString UserName;
            FRegistryManager::Get().GetString(EDevHelperSettingToString(EDevHelperSetting::UserName),UserName);
            Result=Result.Replace(TEXT("@{UserName}"),*UserName,ESearchCase::Type::CaseSensitive);
        }
        // CreateTime
        {
            Result=Result.Replace(TEXT("@{CreateTime}"),*FDateTime::Now().ToString(),ESearchCase::Type::CaseSensitive);
        }
        // ClassName
        {
            FString ClassName=BlueprintName;
            if(LuaFileName.Len())
            {
                ClassName=LuaFileName;
            }
            ClassName+=FileSuffx;
            Result.ReplaceInline(TEXT("@{ClassName}"),*ClassName,ESearchCase::Type::CaseSensitive);
        }

        RichEditableText=FText::FromString(Result);
    }
}

FString SLuaFileInfo::GetTemplateFilePath()
{
    FString TemplateFilePath="";
    UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
    if(IsValid(Setting))
    {
        for (auto LuaInfo:Setting->LuaFileCreationRules)
        {
            if(LuaInfo.Key.Len() && LuaInfo.Key==CheckLuaFileStr)
            {
                TemplateFilePath=FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(),LuaInfo.Value.TemplateFile.FilePath));
                break;
            }
        }
    }
    return TemplateFilePath;
}

FText SLuaFileInfo::GetRichEditableText() const
{
	return RichEditableText;
}

void SLuaFileInfo::HandleRichEditableTextChanged(const FText& Text)
{
	RichEditableText = Text;
}

void SLuaFileInfo::HandleRichEditableTextCommitted(const FText& Text, ETextCommit::Type Type)
{
	RichEditableText = Text;
}

ECheckBoxState SLuaFileInfo::IsEnableSyntaxHighlightingChecked() const
{
	return (SyntaxHighlighterMarshaller->IsSyntaxHighlightingEnabled()) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SLuaFileInfo::OnEnableSyntaxHighlightingChanged(ECheckBoxState InState)
{
	SyntaxHighlighterMarshaller->EnableSyntaxHighlighting(InState == ECheckBoxState::Checked);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION


#undef LOCTEXT_NAMESPACE
