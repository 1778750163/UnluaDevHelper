#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Delegates/Delegate.h"
#include "CustomNotification.generated.h"

UCLASS()
class  UCustomNotification : public UObject
{
    GENERATED_BODY()

public:
    static void ShowTextNotification(const FText& Message,bool bUseSuccessFailIcons=true);

    static void ShowButtonNotification(const FText& Message, const FText& ButtonText,const FSimpleDelegate& ButtonCallback);

    static void ShowLinkNotification(const FText& Message, const FText& LinkText, const FSimpleDelegate& ButtonCallback);
};
