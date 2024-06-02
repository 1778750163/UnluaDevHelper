#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "CodeEditorStyle.h"
#include "Framework/Text/SyntaxTokenizer.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"

class FTextLayout;

/**
 * Get/set the raw text to/from a text layout, and also inject syntax highlighting for our rich-text markup
 */
class FLuaSyntaxHighlighterTextLayoutMarshaller : public FSyntaxHighlighterTextLayoutMarshaller
{
public:

    struct FSyntaxTextStyle
    {
        FSyntaxTextStyle()
            : NormalTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.Normal"))
            , OperatorTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.Operator"))
            , KeywordTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.Keyword"))
            , StringTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.String"))
            , NumberTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.Number"))
            , CommentTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.Comment"))
            , PreProcessorKeywordTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.PreProcessorKeyword"))
            , FunctionNameTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.FunctionName"))
            , VariableNameTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.VariableName"))
            , ParameterTextStyle(FCodeEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Lua.Parameter"))
        {
        }

        FSyntaxTextStyle(const FTextBlockStyle& InNormalTextStyle, const FTextBlockStyle& InOperatorTextStyle, const FTextBlockStyle& InKeywordTextStyle, const FTextBlockStyle& InStringTextStyle, const FTextBlockStyle& InNumberTextStyle, const FTextBlockStyle& InCommentTextStyle, const FTextBlockStyle& InPreProcessorKeywordTextStyle, const FTextBlockStyle& InFunctionNameTextStyle, const FTextBlockStyle& InVariableNameTextStyle, const FTextBlockStyle& InParameterTextStyle)
            : NormalTextStyle(InNormalTextStyle)
            , OperatorTextStyle(InOperatorTextStyle)
            , KeywordTextStyle(InKeywordTextStyle)
            , StringTextStyle(InStringTextStyle)
            , NumberTextStyle(InNumberTextStyle)
            , CommentTextStyle(InCommentTextStyle)
            , PreProcessorKeywordTextStyle(InPreProcessorKeywordTextStyle)
            , FunctionNameTextStyle(InFunctionNameTextStyle)
            , VariableNameTextStyle(InVariableNameTextStyle)
            , ParameterTextStyle(InParameterTextStyle)
        {
        }

        FTextBlockStyle NormalTextStyle;
        FTextBlockStyle OperatorTextStyle;
        FTextBlockStyle KeywordTextStyle;
        FTextBlockStyle StringTextStyle;
        FTextBlockStyle NumberTextStyle;
        FTextBlockStyle CommentTextStyle;
        FTextBlockStyle PreProcessorKeywordTextStyle;
        FTextBlockStyle FunctionNameTextStyle;
        FTextBlockStyle VariableNameTextStyle;
        FTextBlockStyle ParameterTextStyle;
    };

    static TSharedRef< FLuaSyntaxHighlighterTextLayoutMarshaller > Create(const FSyntaxTextStyle& InSyntaxTextStyle);

    virtual ~FLuaSyntaxHighlighterTextLayoutMarshaller();

protected:

    virtual void ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<FSyntaxTokenizer::FTokenizedLine> TokenizedLines) override;

    FLuaSyntaxHighlighterTextLayoutMarshaller(TSharedPtr< FSyntaxTokenizer > InTokenizer, const FSyntaxTextStyle& InSyntaxTextStyle);

    /** Styles used to display the text */
    FSyntaxTextStyle SyntaxTextStyle;

    /** String representing tabs */
    FString TabString;
};
