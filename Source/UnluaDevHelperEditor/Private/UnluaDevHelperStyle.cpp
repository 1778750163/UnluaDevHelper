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
    const FVector2D Icon5x16(5.0f, 16.0f);
    const FVector2D Icon8x4(8.0f, 4.0f);
    const FVector2D Icon8x8(8.0f, 8.0f);
    const FVector2D Icon10x10(10.0f, 10.0f);
    const FVector2D Icon12x12(12.0f, 12.0f);
    const FVector2D Icon12x16(12.0f, 16.0f);
    const FVector2D Icon14x14(14.0f, 14.0f);
    const FVector2D Icon16x16(16.0f, 16.0f);
    const FVector2D Icon20x20(20.0f, 20.0f);
    const FVector2D Icon22x22(22.0f, 22.0f);
    const FVector2D Icon24x24(24.0f, 24.0f);
    const FVector2D Icon25x25(25.0f, 25.0f);
    const FVector2D Icon32x32(32.0f, 32.0f);
    const FVector2D Icon40x40(40.0f, 40.0f);
    const FVector2D Icon64x64(64.0f, 64.0f);
    const FVector2D Icon36x24(36.0f, 24.0f);
    const FVector2D Icon128x128(128.0f, 128.0f);
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