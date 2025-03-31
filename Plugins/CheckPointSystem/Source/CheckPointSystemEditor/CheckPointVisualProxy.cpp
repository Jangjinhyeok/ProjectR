#include "CheckPointVisualProxy.h"

void FCheckPointVisualProxy::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI)
{
	FComponentVisualizer::DrawVisualization(Component, View, PDI);
}

void FCheckPointVisualProxy::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport,
	const FSceneView* View, FCanvas* Canvas)
{
	FComponentVisualizer::DrawVisualizationHUD(Component, Viewport, View, Canvas);
}

bool FCheckPointVisualProxy::VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy,
	const FViewportClick& Click)
{
	return FComponentVisualizer::VisProxyHandleClick(InViewportClient, VisProxy, Click);
}

bool FCheckPointVisualProxy::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
	return FComponentVisualizer::GetWidgetLocation(ViewportClient, OutLocation);
}

bool FCheckPointVisualProxy::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport,
	FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
{
	return FComponentVisualizer::HandleInputDelta(ViewportClient, Viewport, DeltaTranslate, DeltaRotate, DeltaScale);
}
