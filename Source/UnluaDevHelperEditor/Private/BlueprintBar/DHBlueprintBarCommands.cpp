#include "DHBlueprintBarCommands.h"

#define LOCTEXT_NAMESPACE "FBlueprintBarCommands" 

void FDHBlueprintBarCommands::RegisterCommands()
{
    UI_COMMAND(BindLua, "BindLua", "Bind Lua file", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(UnBindLua, "UnBindLua", "UnBind Lua file", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(OpenLua, "OpenLua", "Open Lua file", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CreateLuaFile, "CreateLuaFile", "Create Lua File", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CopyRelativePath, "CopyRelativePath", "Copy Relative Path", EUserInterfaceActionType::Button, FInputChord());
}


#undef LOCTEXT_NAMESPACE