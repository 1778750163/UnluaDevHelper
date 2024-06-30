// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/Classes/Engine/EngineTypes.h"
#include "UnluaDevHelperSetting.generated.h"

UENUM(BlueprintType)
enum class  EIDEType : uint8
{
    VSCode = 0  UMETA(DisplayName="VSCode"),
    IDEA        UMETA(DisplayName="IDEA"),
};


USTRUCT(BlueprintType)
struct  FLuaFileCreationRule
{
    GENERATED_BODY()

public:
    UPROPERTY(Config,EditAnywhere,meta = (FilePathFilter = "lua", RelativeToGameDir))
    FFilePath TemplateFile;

    UPROPERTY(Config,EditAnywhere)
    bool bFixedGenerationPath;
    
    UPROPERTY(Config,EditAnywhere,meta = (FilePathFilter = "lua", RelativeToGameDir,EditCondition  = bFixedGenerationPath ,EditConditionHides))
    FFilePath FixedGenerationPath;
    
    UPROPERTY(Config,EditAnywhere,meta = (FilePathFilter = "lua", RelativeToGameDir,EditCondition  = bFixedGenerationPath ,EditConditionHides))
    TArray<int32> RemoveDirIndexs;
    
    UPROPERTY(Config, EditAnywhere, meta=(DefaultValue = false))
    bool bHasSubDirectory;
    
    UPROPERTY(Config,EditAnywhere,meta=(EditCondition  = bHasSubDirectory ,EditConditionHides))
    FString SubDirectory;

    UPROPERTY(Config, EditAnywhere,meta=(DefaultValue = false))
    bool bHasFileSuffix;
    
    UPROPERTY(Config,EditAnywhere,meta=(EditCondition  = bHasFileSuffix ,EditConditionHides))
    FString FileSuffix;
};


/**
 * 
 */
UCLASS(config=UnluaDevHelperEditor, defaultconfig, meta=(DisplayName="UnluaDevHelperEditor"))
class UNLUADEVHELPEREDITOR_API UUnluaDevHelperSetting : public UObject
{
    GENERATED_BODY()
public:

    UUnluaDevHelperSetting();

    UPROPERTY(Config, EditAnywhere, Category = "LuaFile")
    FString LuaFileDirectory=TEXT("Content/Script");
    
    UPROPERTY(Config, EditAnywhere, Category = "LuaFile")
    TMap<FString,FLuaFileCreationRule> LuaFileCreationRules;
};


static FString GetLuaProjectPath()
{
    UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
    if(!IsValid(Setting)) return FPaths::ProjectDir();
    return FPaths::Combine(FPaths::ProjectDir(),Setting->LuaFileDirectory)  ;
}