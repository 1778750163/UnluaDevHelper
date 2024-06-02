#pragma once
#include "CoreMinimal.h"
#include "RegistryManager.h"
#include "UnluaDevHelperSetting.h"
#include "UnLuaInterface.h"

#include "UnluaDevHelperDefine.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(UnluaDevHelperLog,Log,All)

UCLASS()
class UNLUADEVHELPEREDITOR_API UUnluaDevHelperDefine : public UObject
{
    GENERATED_BODY()
};



UENUM(BlueprintType)
enum class EBlueprintBindState :uint8
{
    Unknown,
    UnBind,
    BindWithoutCreatingFile,
    BindSucceed,
};


static EBlueprintBindState GetBlueprintBindState(const UBlueprint* Blueprint)
{
    if(!IsValid(Blueprint)) return EBlueprintBindState::UnBind;

    if (Blueprint->Status == EBlueprintStatus::BS_Dirty) return  EBlueprintBindState::Unknown;
    
    const auto Target = Blueprint->GeneratedClass;

    if (!IsValid(Target))
        return EBlueprintBindState::UnBind;

    if (!Target->ImplementsInterface(UUnLuaInterface::StaticClass()))
        return EBlueprintBindState::UnBind;
    
    const auto InterfaceDesc = *Blueprint->ImplementedInterfaces.FindByPredicate([](const FBPInterfaceDescription& Desc)
    {
        return Desc.Interface == UUnLuaInterface::StaticClass();
    });

    FString BindPath=InterfaceDesc.Graphs[0]->Nodes[1]->Pins[1]->DefaultValue;

    FString FilePath=FPaths::Combine(GetLuaProjectPath(),FString::Printf(TEXT("%s.lua"),*BindPath));
    
    if(!IFileManager::Get().FileExists(*FPaths::ConvertRelativePathToFull(FilePath)))
    {
        return EBlueprintBindState::BindWithoutCreatingFile;
    }

    return EBlueprintBindState::BindSucceed;
}


