#include "CheckPointSystemEditor.h"

#include "UnrealEdGlobals.h"
#include "CheckPointSystem/Components/CheckPointListComponent.h"
#include "Editor/UnrealEdEngine.h"

#define LOCTEXT_NAMESPACE "FCheckPointSystemEditorModule"

void FCheckPointSystemEditorModule::StartupModule()
{
    const TSharedPtr<FPatrolAgentVisualizer> Visualizer = MakeShareable(new FPatrolAgentVisualizer());
    GUnrealEd->RegisterComponentVisualizer(UCheckPointListComponent::StaticClass()->GetFName(), Visualizer);
}

void FCheckPointSystemEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCheckPointSystemEditorModule, CheckPointSystemEditor)