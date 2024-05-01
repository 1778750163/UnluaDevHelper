#pragma once
#include "Styling/SlateStyle.h"

class FUnluaDevHelperStyle : public FSlateStyleSet
{

public:
    FUnluaDevHelperStyle();
    virtual ~FUnluaDevHelperStyle() override;

    static TSharedPtr<ISlateStyle> GetInstance()
    {
        if (!Instance.IsValid())
            Instance = MakeShared<FUnluaDevHelperStyle>();
        return Instance;
    }

private:
    static TSharedPtr<ISlateStyle> Instance;
};
