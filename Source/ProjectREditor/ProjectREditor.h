#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FProjectREditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedPtr<FUICommandList> CommandList;
};
