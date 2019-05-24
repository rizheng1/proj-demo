
/*=============================================================================
	PostProcessCartoonize.cpp, Created by Rizheng Sun
=============================================================================*/

#include "PostProcess/PostProcessCartoonize.h"
#include "StaticBoundShaderState.h"
#include "SceneUtils.h"
#include "PostProcess/SceneRenderTargets.h"
#include "PostProcess/SceneFilterRendering.h"
#include "PostProcess/PostProcessing.h"
#include "ClearQuad.h"
#include "PipelineStateCache.h"

class FPostProcessCartoonizePS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FPostProcessCartoonizePS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM4);
	}

	/** Default constructor. */
	FPostProcessCartoonizePS() {}

public:
	FShaderParameter MyColorParameter;
	FPostProcessPassParameters PostprocessParameter;
	/** Initialization constructor. */
	FPostProcessCartoonizePS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		PostprocessParameter.Bind(Initializer.ParameterMap);
	}

	
	// FShader interface.
	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << PostprocessParameter << MyColorParameter;
		return bShaderHasOutdatedParameters;
	}

	template <typename TRHICmdList>
	void SetParameters(TRHICmdList& RHICmdList, const FRenderingCompositePassContext& Context)
	{
		const FPixelShaderRHIParamRef ShaderRHI = GetPixelShader();

		FGlobalShader::SetParameters<FViewUniformShaderParameters>(Context.RHICmdList, ShaderRHI, Context.View.ViewUniformBuffer);

		PostprocessParameter.SetPS(RHICmdList, ShaderRHI, Context, TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI());

	}

	static const TCHAR* GetSourceFilename()
	{
		return TEXT("/Engine/Private/PostProcessCartoonize.usf");
	}

	static const TCHAR* GetFunctionName()
	{
		return TEXT("MainPS");
	}
};

IMPLEMENT_SHADER_TYPE(, FPostProcessCartoonizePS, TEXT("/Engine/Private/PostProcessCartoonize.usf"), TEXT("MainPS"), SF_Pixel);

void FRCPassPostProcessCartoonize::Process(FRenderingCompositePassContext& Context)
{
	SCOPED_DRAW_EVENT(Context.RHICmdList, Cartoonize);

	const FRenderingCompositeOutputRef* Input = GetInput(ePId_Input0);

	const FPooledRenderTargetDesc* InputDesc1 = GetInputDesc(ePId_Input0);
	const FPooledRenderTargetDesc* InputDesc2 = GetInputDesc(ePId_Input1);
	
	if (!Input)
	{
		// input is not hooked up correctly
		return;
	}
	const FViewInfo& View = Context.View;

	FIntPoint TexSize1 = InputDesc1->Extent;
	FIntPoint TexSize2 = InputDesc2->Extent;

	uint32 ScaleToFullRes1 = Context.ReferenceBufferSize.X / TexSize1.X;
	uint32 ScaleToFullRes2 = Context.ReferenceBufferSize.X / TexSize2.X;

	FIntRect ViewRect1 = FIntRect::DivideAndRoundUp(Context.SceneColorViewRect, ScaleToFullRes1);
	FIntRect ViewRect2 = FIntRect::DivideAndRoundUp(Context.SceneColorViewRect, ScaleToFullRes2);
	FIntPoint ViewSize1 = ViewRect1.Size();
	FIntPoint ViewSize2 = ViewRect2.Size();

	const FSceneRenderTargetItem& DestRenderTarget = PassOutputs[0].RequestSurface(Context);

	FIntPoint DestSize = PassOutputs[0].RenderTargetDesc.Extent;

	//Turns On Lights
	SetRenderTarget(Context.RHICmdList, DestRenderTarget.TargetableTexture, FTextureRHIRef());
	Context.SetViewportAndCallRHI(0, 0, 0.0f, DestSize.X, DestSize.Y, 1.0f);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;	
	Context.RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();


	
	TShaderMapRef<FPostProcessVS> VertexShader(Context.GetShaderMap());
	TShaderMapRef<FPostProcessCartoonizePS> PixelShader(Context.GetShaderMap());

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(Context.RHICmdList, GraphicsPSOInit);



	VertexShader->SetParameters(Context);
	PixelShader->SetParameters(Context.RHICmdList, Context);
	

	FRHIRenderTargetView RtView = FRHIRenderTargetView(DestRenderTarget.TargetableTexture, ERenderTargetLoadAction::ENoAction);
	FRHISetRenderTargetsInfo Info(1, &RtView, FRHIDepthRenderTargetView());
	Context.RHICmdList.SetRenderTargetsAndClear(Info);

	Context.SetViewportAndCallRHI(ViewRect1);


	
	
	FLinearColor FlareColor = Context.View.FinalPostProcessSettings.LensFlareTints[0 % 8];
	float NormalizedAlpha = FlareColor.A;
	float Alpha = NormalizedAlpha * 7.0f - 3.5f;




	FVector2D Center = FVector2D(ViewSize1) * 0.5f;

	DrawPostProcessPass(
		Context.RHICmdList,
		ViewRect1.Min.X, ViewRect1.Min.Y,
		ViewSize1.X, ViewSize1.Y,
		ViewRect2.Min.X, ViewRect2.Min.Y,
		ViewSize2.X, ViewSize2.Y,
		ViewSize1,
		TexSize2,
		*VertexShader,
		View.StereoPass,
		Context.HasHmdMesh(),
		EDRF_UseTriangleOptimization);


	Context.RHICmdList.CopyToResolveTarget(DestRenderTarget.TargetableTexture, DestRenderTarget.ShaderResourceTexture, FResolveParams());
}

FRCPassPostProcessCartoonize::FRCPassPostProcessCartoonize()
{
}

FPooledRenderTargetDesc FRCPassPostProcessCartoonize::ComputeOutputDesc(EPassOutputId InPassOutputId) const
{
	FPooledRenderTargetDesc Ret = GetInput(ePId_Input0)->GetOutput()->RenderTargetDesc;

	Ret.Reset();
	Ret.DebugName = TEXT("Cartoonize");

	return Ret;
}
