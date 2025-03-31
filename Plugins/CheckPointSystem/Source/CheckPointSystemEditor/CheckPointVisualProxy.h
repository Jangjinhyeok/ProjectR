#pragma once
#include "ComponentVisualizer.h"

class UCheckPointListComponent;

class CHECKPOINTSYSTEMEDITOR_API FCheckPointVisualProxy : public FComponentVisualizer
{
public:
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;

	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;

	// 약한 참조로 없는 경우 알아서 GC에 수집되도록 처리
	// 해당 위치에 Binding되는 것이 아닌 단순한 참조이기 때문
	TWeakObjectPtr<const UCheckPointListComponent> CheckPointListComponent;
	int32 SelectedProxyId;
};
