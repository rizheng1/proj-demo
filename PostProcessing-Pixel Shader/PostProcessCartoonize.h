
/*=============================================================================
	PostProcessCartoonize.h, Created by Rizheng Sun
=============================================================================*/

#pragma once

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "PostProcess/RenderingCompositionGraph.h"

// ePId_Input0: SceneColor
// ePId_Input1: DepthMap
class FRCPassPostProcessCartoonize : public TRenderingCompositePassBase<2, 1>
{
public:
	// constructor
	FRCPassPostProcessCartoonize();


	virtual void Process(FRenderingCompositePassContext& Context) override;
	virtual void Release() override { delete this; }
	virtual FPooledRenderTargetDesc ComputeOutputDesc(EPassOutputId InPassOutputId) const override;

private:

};

