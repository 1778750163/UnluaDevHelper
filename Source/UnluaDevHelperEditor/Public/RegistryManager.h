#pragma once

#include "CoreMinimal.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

UENUM()
enum class EDevHelperSetting :uint8
{
    LuaFileDirectory,
    IDEType,
    bEnableDebug,
};

FString static EDevHelperSettingToString( EDevHelperSetting v )
{
    const char* Ret = ""; switch ( v )
    {
    case EDevHelperSetting::LuaFileDirectory:   Ret = "LuaFileDirectory";   break;
    case EDevHelperSetting::IDEType:            Ret = "IDEType";            break;
    case EDevHelperSetting::bEnableDebug:       Ret = "bEnableDebug";       break;
    }

    return Ret;
}
UENUM(BlueprintType)
enum class  EIDEType : uint8
{
    VSCode = 0  UMETA(DisplayName="VSCode"),
    IDEA        UMETA(DisplayName="IDEA"),
};

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


static FString GetLuaProjectPath()
{
    FString LuaFileDirectory;
    if(!FRegistryManager::Get().GetString(EDevHelperSettingToString(EDevHelperSetting::LuaFileDirectory),LuaFileDirectory) || LuaFileDirectory.Len()==0)
    {
        LuaFileDirectory=TEXT("Content/Script");
        FRegistryManager::Get().SetString(EDevHelperSettingToString(EDevHelperSetting::LuaFileDirectory),LuaFileDirectory);
    }
    return FPaths::Combine(FPaths::ProjectDir(),LuaFileDirectory)  ;
}


