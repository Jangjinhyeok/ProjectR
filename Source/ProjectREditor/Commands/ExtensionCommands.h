#pragma once

class FExtensionTestActions
{
public:
	static void Action1();
	static void Action2();
	static void Action3();
};

class FExtensionCommands : public TCommands<FExtensionCommands>
{
public:
	FExtensionCommands(): TCommands<FExtensionCommands>(
		TEXT("Rider"), NSLOCTEXT("Rider", "Rider", "Rider")
		, NAME_None, TEXT("Default"))
	{
		
	}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> TestCmd1;
	TSharedPtr<FUICommandInfo> TestCmd2;
	TSharedPtr<FUICommandInfo> TestCmd3;
	TSharedPtr<FUICommandInfo> TestCmd4;
};
