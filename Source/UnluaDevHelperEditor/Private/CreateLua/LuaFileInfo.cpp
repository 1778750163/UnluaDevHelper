// Fill out your copyright notice in the Description page of Project Settings.


#include "LuaFileInfo.h"

#include "LuaRichTextSyntaxHighlighterTextLayoutMarshaller.h"
#include "SlateOptMacros.h"
#include "TestStyle.h"
#include "UnluaDevHelperSetting.h"
#include "UnluaDevHelperStyle.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"

#define LOCTEXT_NAMESPACE "SLuaFileInfo"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLuaFileInfo::Construct(const FArguments& InArgs)
{
	BlueprintPath=InArgs._BlueprintPath.Get();
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
		.HAlign(HAlign_Left)
		.Padding(10.0f, 10.0f, 10.0f, 0.0f)
		[
			SNew(STextBlock)
			.Text(this, &SLuaFileInfo::OnFilePath)
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
	if(TextComboBox.IsValid())
	{
		TextComboBox->SetSelectedItem(SpeedSettingStrings[0]);
	}
}

FReply SLuaFileInfo::OnCreateLuaFileButtonClicked()
{
	UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
	if(IsValid(Setting))
	{
		for (auto LuaInfo:Setting->LuaFileCreationRules)
		{
			if(LuaInfo.Key.Len() && LuaInfo.Key==CheckLuaFileStr)
			{
				
			}
		}
	}
	return FReply::Handled();
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

FText SLuaFileInfo::OnFilePath() const
{
	return FText::FromString(BlueprintPath);
	return FText::FromString(FilePath);
}

void SLuaFileInfo::RefreshFilePath()
{
	FString Path=FPaths::ConvertRelativePathToFull(GetLuaProjectPath());
	FString SubPath="";
	FString FileSuffx="";
	UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
	if(IsValid(Setting))
	{
		for (auto LuaInfo:Setting->LuaFileCreationRules)
		{
			if(LuaInfo.Key.Len() && LuaInfo.Key==CheckLuaFileStr)
			{
				SubPath=LuaInfo.Value.bHasSubDirectory?LuaInfo.Value.SubDirectory:"";
				FileSuffx=LuaInfo.Value.bHasFileSuffix?LuaInfo.Value.FileSuffix:"";
				break;
			}
		}
	}
	Path = SubPath.Len() > 0 ? FPaths::Combine(Path, SubPath) : Path;

	if(LuaFileName.Len())
	{
		FilePath=FPaths::Combine(Path,LuaFileName)+FileSuffx+".lua";
	}
	else
	{
		FilePath=Path;
	}
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
