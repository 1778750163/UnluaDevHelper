#include "RegistryManager.h"

FRegistryManager& FRegistryManager::Get() {
    static FRegistryManager instance(HKEY_CURRENT_USER, TEXT("Software\\UnLuaDevHelper"));
    return instance;
}

FRegistryManager::FRegistryManager(HKEY root, const FString& subkey) {
    if (RegCreateKeyEx(root, *subkey, 0, NULL, REG_OPTION_NON_VOLATILE,
                       KEY_ALL_ACCESS, NULL, &m_hKey, NULL) != ERROR_SUCCESS) {
        m_hKey = NULL;
    }
}

FRegistryManager::~FRegistryManager() {
    if (m_hKey != NULL) {
        RegCloseKey(m_hKey);
    }
}

bool FRegistryManager::SetInt(const FString& name, int32 value) {
    if (m_hKey == NULL) return false;
    return RegSetValueEx(m_hKey, *name, 0, REG_DWORD,
                         reinterpret_cast<const BYTE*>(&value), sizeof(value)) == ERROR_SUCCESS;
}

bool FRegistryManager::SetString(const FString& name, const FString& value) {
    if (m_hKey == NULL) return false;
    return RegSetValueEx(m_hKey, *name, 0, REG_SZ,
                         reinterpret_cast<const BYTE*>(*value),
                         (value.Len() + 1) * sizeof(TCHAR)) == ERROR_SUCCESS;
}

bool FRegistryManager::SetBool(const FString& name, bool value) {
    return SetInt(name, value ? 1 : 0);
}

bool FRegistryManager::GetInt(const FString& name, int32& value) {
    if (m_hKey == NULL) return false;
    DWORD dataSize = sizeof(value);
    // Pass the address of dataSize as the fourth parameter
    DWORD type = 0; // Additional variable to store the type of data retrieved
    return RegQueryValueEx(m_hKey, *name, 0, &type,
                           reinterpret_cast<LPBYTE>(&value), &dataSize) == ERROR_SUCCESS;
}

bool FRegistryManager::GetString(const FString& name, FString& value) {
    if (m_hKey == NULL) return false;
    DWORD dataSize;
    // Correctly passing the address of dataSize
    DWORD type = 0; // To store the type of data, expecting REG_SZ
    if (RegQueryValueEx(m_hKey, *name, 0, &type, NULL, &dataSize) != ERROR_SUCCESS) {
        return false;
    }
    TCHAR* buffer = new TCHAR[dataSize / sizeof(TCHAR)];
    if (RegQueryValueEx(m_hKey, *name, 0, &type,
                        reinterpret_cast<LPBYTE>(buffer), &dataSize) == ERROR_SUCCESS) {
        value = FString(buffer);
        delete[] buffer;
        return true;
                        }
    delete[] buffer;
    return false;
}

bool FRegistryManager::GetBool(const FString& name, bool& value) {
    int32 intValue;
    if (GetInt(name, intValue)) {
        value = (intValue != 0);
        return true;
    }
    return false;
}
