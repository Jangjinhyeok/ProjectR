#include "ExtensionCommands.h"

#define LOCTEXT_NAMESPACE "RiderCmd"

void FExtensionCommands::RegisterCommands()
{
	UI_COMMAND(TestCmd1, "TestCmd1", "Test ToolTip1", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(TestCmd2, "TestCmd2", "Test ToolTip2", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(TestCmd3, "TestCmd3", "Test ToolTip3", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(TestCmd4, "TestCmd4", "Test ToolTip4", EUserInterfaceActionType::ToggleButton, FInputGesture());
}

void FExtensionTestActions::Action1()
{
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RiderCmd", "테스트 완료"));
}

void FExtensionTestActions::Action2()
{
	FMessageDialog::Open(EAppMsgType::YesNoCancel, LOCTEXT("RiderCmd", "어서오고"));
}

void FExtensionTestActions::Action3()
{
	FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("RiderCmd", "뭘봐"));
}

#undef LOCTEXT_NAMESPACE