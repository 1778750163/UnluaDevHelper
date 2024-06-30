#include "CustomNotification.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void UCustomNotification::ShowTextNotification(const FText& Message,bool bUseSuccessFailIcons)
{
    FNotificationInfo Info(Message);
    Info.bFireAndForget = true;
    Info.FadeOutDuration = 3.0f;
    Info.ExpireDuration = 2.0f;
    Info.bUseSuccessFailIcons = bUseSuccessFailIcons;
    
    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
    if (Notification.IsValid())
    {
        Notification->SetCompletionState(bUseSuccessFailIcons?SNotificationItem::CS_Success:SNotificationItem::CS_Fail);
    }
}

void UCustomNotification::ShowButtonNotification(const FText& Message, const FText& ButtonText,const FSimpleDelegate& ButtonCallback)
{
    FNotificationInfo Info(Message);
    Info.bFireAndForget = true;
    Info.FadeOutDuration = 3.0f;
    Info.ExpireDuration = 3.0f;
    Info.bUseSuccessFailIcons = true;
    Info.ButtonDetails.Add(FNotificationButtonInfo(ButtonText, FText::GetEmpty(), ButtonCallback));

    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
    if (Notification.IsValid())
    {
        Notification->SetCompletionState(SNotificationItem::CS_Success);
    }
}

void UCustomNotification::ShowLinkNotification(const FText& Message, const FText& LinkText,
   const FSimpleDelegate& ButtonCallback)
{
    FNotificationInfo Info(Message);
    Info.bFireAndForget = true;
    Info.FadeOutDuration = 3.0f;
    Info.ExpireDuration = 3.0f;
    Info.bUseSuccessFailIcons = true;
    Info.HyperlinkText=LinkText;
    Info.Hyperlink=ButtonCallback;

    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
    if (Notification.IsValid())
    {
        Notification->SetCompletionState(SNotificationItem::CS_Success);
    }
}
