
#include "MainMenuBarCommands.h"

#define LOCTEXT_NAMESPACE "FMainMenuBarCommands"

void FMainMenuBarCommands::RegisterCommands()
{
    UI_COMMAND(OpenSoulation, "Open Soulation", "Open lua Soulation", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(SelectVSCode, "Change IDE Type", "Select VSCode.", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_COMMAND(SelectIDEA, "Change IDE Type", "Select IDEA.",EUserInterfaceActionType::RadioButton, FInputChord());
    UI_COMMAND(EnableDebug, "Enable Debug", "Enable Debug.",EUserInterfaceActionType::ToggleButton, FInputChord());
    UI_COMMAND(ChoosePath, "Choose Path", "Choose a directory path", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(TcpListen, "Change Idea TcpListen", "TcpListen", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_COMMAND(TcpConnect, "Change Idea TcpConnect", "TcpConnect",EUserInterfaceActionType::RadioButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
