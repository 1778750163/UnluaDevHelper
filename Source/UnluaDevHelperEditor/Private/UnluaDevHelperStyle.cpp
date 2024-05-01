#include "UnluaDevHelperStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"


#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(RootToContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define OTF_FONT(RelativePath, ...) FSlateFontInfo(RootToContentDir(RelativePath, TEXT(".otf")), __VA_ARGS__)

TSharedPtr<ISlateStyle> FUnluaDevHelperStyle::Instance = nullptr;

FUnluaDevHelperStyle::FUnluaDevHelperStyle():
    FSlateStyleSet(TEXT("UnluaDevHelperStyle"))
{
    const FVector2D Icon40x40(40.0f, 40.0f);

    FSlateStyleSet::SetContentRoot(IPluginManager::Get().FindPlugin("UnluaDevHelper")->GetBaseDir() / TEXT("Resources"));
    
    Set("UnluaDevHelperEditor.Logo", new IMAGE_BRUSH("Icons/logo40x40", Icon40x40));
    Set("UnluaDevHelperEditor.LogoError", new IMAGE_BRUSH("Icons/logo40x40_error", Icon40x40));
    Set("UnluaDevHelperEditor.LogoWarning", new IMAGE_BRUSH("Icons/logo40x40_warning", Icon40x40));
    Set("UnluaDevHelperEditor.LogoSucceed", new IMAGE_BRUSH("Icons/logo40x40_succeed", Icon40x40));
    
    FSlateStyleRegistry::RegisterSlateStyle(*this);
}

FUnluaDevHelperStyle::~FUnluaDevHelperStyle()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}


#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT