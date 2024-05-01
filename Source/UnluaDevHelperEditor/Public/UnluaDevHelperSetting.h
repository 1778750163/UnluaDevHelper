// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
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
    UPROPERTY(Config,EditAnywhere,DisplayName=TemplateFile,meta = (FilePathFilter = "lua", RelativeToGameDir))
    FFilePath TemplateFile;

    UPROPERTY(Config, EditAnywhere, DisplayName=bHasSubDirectory, meta=(DefaultValue = false))
    bool bHasSubDirectory;
    
    UPROPERTY(Config,EditAnywhere,DisplayName=Subdirectory,meta=(EditCondition  = bHasSubDirectory ,EditConditionHides))
    FString SubDirectory;

    UPROPERTY(Config, EditAnywhere, DisplayName=bHasFileSuffix, meta=(DefaultValue = false))
    bool bHasFileSuffix;
    
    UPROPERTY(Config,EditAnywhere,DisplayName=FileSuffix,meta=(EditCondition  = bHasFileSuffix ,EditConditionHides))
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

    void SetLuaFileDirectory();
    
    UPROPERTY(Config, EditAnywhere, Category = "Setting",meta=(DefaultValue = 0))
    EIDEType IDEType;

    UPROPERTY(Config, EditAnywhere, Category = "Setting",meta=(DefaultValue = true))
    bool bEnableDebug;

    UPROPERTY(Config, EditAnywhere, Category = "LuaFile")
    FString LuaFileDirectory;
    
    UPROPERTY(Config, EditAnywhere, Category = "LuaFile")
    TArray<FLuaFileCreationRule> LuaFileCreationRules;
};


static FString GetLuaProjectPath()
{
    UUnluaDevHelperSetting* Setting=GetMutableDefault<UUnluaDevHelperSetting>();
    if(!IsValid(Setting)) return FPaths::ProjectContentDir();
    return FPaths::Combine(FPaths::ProjectContentDir(),Setting->LuaFileDirectory)  ;
}