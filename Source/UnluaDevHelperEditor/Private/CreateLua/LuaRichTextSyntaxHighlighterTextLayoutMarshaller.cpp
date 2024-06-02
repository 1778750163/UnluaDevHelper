#include "LuaRichTextSyntaxHighlighterTextLayoutMarshaller.h"

#include "WhiteSpaceTextRun.h"
#include "Framework/Text/IRun.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "Misc/ExpressionParserTypes.h"

const TCHAR* Keywords[] =
{
    TEXT("and"),
    TEXT("break"),
    TEXT("do"),
    TEXT("else"),
    TEXT("elseif"),
    TEXT("end"),
    TEXT("false"),
    TEXT("for"),
    TEXT("function"),
    TEXT("goto"),
    TEXT("if"),
    TEXT("in"),
    TEXT("local"),
    TEXT("nil"),
    TEXT("not"),
    TEXT("or"),
    TEXT("repeat"),
    TEXT("return"),
    TEXT("then"),
    TEXT("true"),
    TEXT("until"),
    TEXT("while")
};

const TCHAR* Operators[] =
{
    TEXT("--[["),
    TEXT("--"),
    TEXT("]]"),
    TEXT("+"),
    TEXT("-"),
    TEXT("*"),
    TEXT("/"),
    TEXT("%"),
    TEXT("^"),
    TEXT("#"),
    TEXT("=="),
    TEXT("~="),
    TEXT("<="),
    TEXT(">="),
    TEXT("<"),
    TEXT(">"),
    TEXT("="),
    TEXT("("),
    TEXT(")"),
    TEXT("{"),
    TEXT("}"),
    TEXT("["),
    TEXT("]"),
    TEXT(";"),
    TEXT(":"),
    TEXT(","),
    TEXT("."),
    TEXT(".."),
    TEXT("..."),
};

TSharedRef< FLuaSyntaxHighlighterTextLayoutMarshaller > FLuaSyntaxHighlighterTextLayoutMarshaller::Create(const FSyntaxTextStyle& InSyntaxTextStyle)
{
    TArray<FSyntaxTokenizer::FRule> TokenizerRules;

    for (const auto& Operator : Operators)
    {
        TokenizerRules.Emplace(FSyntaxTokenizer::FRule(Operator));
    }

    for (const auto& Keyword : Keywords)
    {
        TokenizerRules.Emplace(FSyntaxTokenizer::FRule(Keyword));
    }

    return MakeShareable(new FLuaSyntaxHighlighterTextLayoutMarshaller(FSyntaxTokenizer::Create(TokenizerRules), InSyntaxTextStyle));
}

FLuaSyntaxHighlighterTextLayoutMarshaller::~FLuaSyntaxHighlighterTextLayoutMarshaller()
{
}

void FLuaSyntaxHighlighterTextLayoutMarshaller::ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<FSyntaxTokenizer::FTokenizedLine> TokenizedLines)
{
    enum class EParseState : uint8
    {
        None,
        LookingForString,
        LookingForCharacter,
        LookingForSingleLineComment,
        LookingForMultiLineComment,
        LookingForFunctionName,
        LookingForVariableName,
        LookingForParameter
    };

    TArray<FTextLayout::FNewLineData> LinesToAdd;
    LinesToAdd.Reserve(TokenizedLines.Num());

    EParseState ParseState = EParseState::None;
    for (const FSyntaxTokenizer::FTokenizedLine& TokenizedLine : TokenizedLines)
    {
        TSharedRef<FString> ModelString = MakeShareable(new FString());
        TArray< TSharedRef< IRun > > Runs;

        if (ParseState == EParseState::LookingForSingleLineComment)
        {
            ParseState = EParseState::None;
        }

        for (const FSyntaxTokenizer::FToken& Token : TokenizedLine.Tokens)
        {
            const FString TokenText = SourceString.Mid(Token.Range.BeginIndex, Token.Range.Len());

            const FTextRange ModelRange(ModelString->Len(), ModelString->Len() + TokenText.Len());
            ModelString->Append(TokenText);

            FRunInfo RunInfo(TEXT("SyntaxHighlight.Lua.Normal"));
            FTextBlockStyle TextBlockStyle = SyntaxTextStyle.NormalTextStyle;

            const bool bIsWhitespace = FString(TokenText).TrimEnd().IsEmpty();
            if (!bIsWhitespace)
            {
                bool bHasMatchedSyntax = false;
                if (Token.Type == FSyntaxTokenizer::ETokenType::Syntax)
                {
                    if (ParseState == EParseState::None && TokenText == TEXT("\""))
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.String");
                        TextBlockStyle = SyntaxTextStyle.StringTextStyle;
                        ParseState = EParseState::LookingForString;
                        bHasMatchedSyntax = true;
                    }
                    else if (ParseState == EParseState::LookingForString && TokenText == TEXT("\""))
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Normal");
                        TextBlockStyle = SyntaxTextStyle.StringTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::None && TokenText == TEXT("\'"))
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.String");
                        TextBlockStyle = SyntaxTextStyle.StringTextStyle;
                        ParseState = EParseState::LookingForCharacter;
                        bHasMatchedSyntax = true;
                    }
                    else if (ParseState == EParseState::LookingForCharacter && TokenText == TEXT("\'"))
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Normal");
                        TextBlockStyle = SyntaxTextStyle.StringTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::None && TokenText.StartsWith(TEXT("--")))
                    {
                        if (TokenText.StartsWith(TEXT("--[[")))
                        {
                            RunInfo.Name = TEXT("SyntaxHighlight.Lua.Comment");
                            TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
                            ParseState = EParseState::LookingForMultiLineComment;
                        }
                        else
                        {
                            RunInfo.Name = TEXT("SyntaxHighlight.Lua.Comment");
                            TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
                            ParseState = EParseState::LookingForSingleLineComment;
                        }
                    }
                    else if (ParseState == EParseState::LookingForMultiLineComment && TokenText.EndsWith(TEXT("]]")))
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Comment");
                        TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::None && TChar<WIDECHAR>::IsAlpha(TokenText[0]))
                    {
                        if (TokenText == TEXT("function"))
                        {
                            ParseState = EParseState::LookingForFunctionName;
                        }
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Keyword");
                        TextBlockStyle = SyntaxTextStyle.KeywordTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::None && !TChar<WIDECHAR>::IsAlpha(TokenText[0]))
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Operator");
                        TextBlockStyle = SyntaxTextStyle.OperatorTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::LookingForFunctionName)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.FunctionName");
                        TextBlockStyle = SyntaxTextStyle.FunctionNameTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::LookingForVariableName)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.VariableName");
                        TextBlockStyle = SyntaxTextStyle.VariableNameTextStyle;
                        ParseState = EParseState::None;
                    }
                    else if (ParseState == EParseState::LookingForParameter)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Parameter");
                        TextBlockStyle = SyntaxTextStyle.ParameterTextStyle;
                        ParseState = EParseState::None;
                    }
                }

                if (Token.Type == FSyntaxTokenizer::ETokenType::Literal || !bHasMatchedSyntax)
                {
                    if (ParseState == EParseState::LookingForString)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.String");
                        TextBlockStyle = SyntaxTextStyle.StringTextStyle;
                    }
                    else if (ParseState == EParseState::LookingForCharacter)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.String");
                        TextBlockStyle = SyntaxTextStyle.StringTextStyle;
                    }
                    else if (ParseState == EParseState::LookingForSingleLineComment)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Comment");
                        TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
                    }
                    else if (ParseState == EParseState::LookingForMultiLineComment)
                    {
                        RunInfo.Name = TEXT("SyntaxHighlight.Lua.Comment");
                        TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
                    }
                }

                TSharedRef< ISlateRun > Run = FSlateTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange);
                Runs.Add(Run);
            }
            else
            {
                RunInfo.Name = TEXT("SyntaxHighlight.Lua.WhiteSpace");
                TSharedRef< ISlateRun > Run = FWhiteSpaceTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, 4);
                Runs.Add(Run);
            }
        }

        LinesToAdd.Emplace(MoveTemp(ModelString), MoveTemp(Runs));
    }

    TargetTextLayout.AddLines(LinesToAdd);
}

FLuaSyntaxHighlighterTextLayoutMarshaller::FLuaSyntaxHighlighterTextLayoutMarshaller(TSharedPtr< FSyntaxTokenizer > InTokenizer, const FSyntaxTextStyle& InSyntaxTextStyle)
    : FSyntaxHighlighterTextLayoutMarshaller(MoveTemp(InTokenizer))
    , SyntaxTextStyle(InSyntaxTextStyle)
{
}
