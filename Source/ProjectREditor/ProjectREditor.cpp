#include "ProjectREditor.h"

#include "LevelEditor.h"
#include "Commands/ExtensionCommands.h"

#define LOCTEXT_NAMESPACE "FProjectREditorModule"

void FProjectREditorModule::StartupModule()
{
	// 커맨드 등록.
	FExtensionCommands::Register();
	// 커맨드와 액션을 서로 묶어주기.
	CommandList = MakeShareable(new FUICommandList());
	// static 형태의 싱글톤 객체를 가져오고 그 안의 Command ptr을 가져와서 바인딩 하는 방식
	CommandList->MapAction(FExtensionCommands::Get().TestCmd1,
	FExecuteAction::CreateStatic(&FExtensionTestActions::Action1), FCanExecuteAction());
	CommandList->MapAction(FExtensionCommands::Get().TestCmd2,
	FExecuteAction::CreateStatic(&FExtensionTestActions::Action2), FCanExecuteAction());
	CommandList->MapAction(FExtensionCommands::Get().TestCmd3,
	FExecuteAction::CreateStatic(&FExtensionTestActions::Action3), FCanExecuteAction());
	CommandList->MapAction(FExtensionCommands::Get().TestCmd4,
		FExecuteAction::CreateStatic(&FExtensionTestActions::Action1), FCanExecuteAction());

	// 메뉴 생성을 위한 델리게이트 함수 선언.
	struct FMenu
	{
		static void CreateMenu(FMenuBuilder& MenuBuilder)
		{
			// 첫 번째 섹션.
			MenuBuilder.BeginSection("Section1", LOCTEXT("Menu", " Menu Section1"));

			MenuBuilder.AddMenuEntry(FExtensionCommands::Get().TestCmd1);
			MenuBuilder.AddMenuEntry(FExtensionCommands::Get().TestCmd2);

			MenuBuilder.EndSection();

			// 두 번째 섹션.
			MenuBuilder.BeginSection("Section2", LOCTEXT("Menu", " Menu Section2"));

			{
				// 서브 메뉴 생성을 위한 델리게이트 함수 선언.
				struct FSubMenu
				{
					static void CreateSubMenu(FMenuBuilder& SubMenuBuilder)
					{
						SubMenuBuilder.AddMenuEntry(FExtensionCommands::Get().TestCmd3);
						SubMenuBuilder.AddMenuEntry(FExtensionCommands::Get().TestCmd4);
					}
				};

				// 서브 메뉴 등록.
				MenuBuilder.AddSubMenu(
					LOCTEXT("Menu", " SubMenu"),
					LOCTEXT("Menu", " SubMenu Tooltip"),
					FNewMenuDelegate::CreateStatic(&FSubMenu::CreateSubMenu), false, FSlateIcon());
			}

			MenuBuilder.EndSection();
		}
	};
	// 레벨 에디터 얻어오기. 
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender> IGCMenuExtender = MakeShareable(new FExtender());
	IGCMenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, CommandList,
		FMenuExtensionDelegate::CreateStatic(&FMenu::CreateMenu));

	struct FToolbar
	{
		static void CreateIGCToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Toolbar");
			{
				ToolbarBuilder.AddToolBarButton(FExtensionCommands::Get().TestCmd1);
				ToolbarBuilder.AddToolBarButton(FExtensionCommands::Get().TestCmd2);
				ToolbarBuilder.AddToolBarButton(FExtensionCommands::Get().TestCmd3);
				ToolbarBuilder.AddToolBarButton(FExtensionCommands::Get().TestCmd4);
			}
			ToolbarBuilder.EndSection();
		}
	};
	
	// 메뉴 추가.
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(IGCMenuExtender);
	
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender());
	ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, CommandList,
		FToolBarExtensionDelegate::CreateStatic(&FToolbar::CreateIGCToolbar));

	// 툴바 추가.
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FProjectREditorModule::ShutdownModule()
{
	FExtensionCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FProjectREditorModule, ProjectREditor)