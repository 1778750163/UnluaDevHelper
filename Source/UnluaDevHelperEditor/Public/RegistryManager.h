#pragma once

#include "CoreMinimal.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

UENUM()
enum class EDevHelperSetting :uint8
{
    IDEType,
    bEnableDebug,
    UserName,
    IdeaName,
    IdeaDebugMode
};

UENUM()
enum class EIdeaDebugMode:uint8
{
    TcpListen,
    TcpConnect
};

FString static EDevHelperSettingToString( EDevHelperSetting v )
{
    const char* Ret = ""; switch ( v )
    {
    case EDevHelperSetting::IDEType:            Ret = "IDEType";            break;
    case EDevHelperSetting::bEnableDebug:       Ret = "bEnableDebug";       break;
    case EDevHelperSetting::UserName:           Ret = "UserName";           break;
    case EDevHelperSetting::IdeaName:           Ret = "IdeaName";    break;
    case EDevHelperSetting::IdeaDebugMode:           Ret = "IdeaDebugMode";    break;
    }

    return Ret;
}

class FRegistryManager {
public:
    static FRegistryManager& Get();

    bool SetInt(const FString& name, int32 value);
    bool SetString(const FString& name, const FString& value);
    bool SetBool(const FString& name, bool value);
    template<typename TEnum>
    bool SetEnum(const FString& name, TEnum value);

    bool GetInt(const FString& name, int32& value);
    bool GetString(const FString& name, FString& value);
    bool GetBool(const FString& name, bool& value);
    template<typename TEnum>
    bool GetEnum(const FString& name, TEnum& value);
private:
    HKEY m_hKey;

    FRegistryManager(HKEY root, const FString& subkey);  // Private constructor
    ~FRegistryManager();  // Private destructor

    // Deleted copy constructor and assignment operator
    FRegistryManager(const FRegistryManager&) = delete;
    FRegistryManager& operator=(const FRegistryManager&) = delete;
};

template<typename TEnum>
bool FRegistryManager::SetEnum(const FString& name, TEnum value) {
    static_assert(std::is_enum<TEnum>::value, "TEnum must be an enum type");
    return SetInt(name, static_cast<int32>(value));
}

template<typename TEnum>
bool FRegistryManager::GetEnum(const FString& name, TEnum& value) {
    static_assert(std::is_enum<TEnum>::value, "TEnum must be an enum type");
    int32 intValue;
    if (GetInt(name, intValue)) {
        value = static_cast<TEnum>(intValue);
        return true;
    }
    return false;
}
