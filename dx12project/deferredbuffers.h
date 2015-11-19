#ifndef _DEFERREDBUFFERS_H_
#define _DEFERREDBUFFERS_H_

//number of rendering buffers
#define BUFFER_COUNT 2

#include <d3d12.h>
#include <DirectXMath.h>

using namespace DirectX;

class DeferredBuffers{
	int m_textureWidth, m_textureHeight;

	ID3D12Texture2D* m_renderTargetTextureArray[BUFFER_COUNT];
	ID3D12RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
	ID3D12ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];
	ID3D12Resource Texture2D* m_depthStencilBuffer;
	ID3D12DepthStencilView* m_depthStencilView;
	D3D12_VIEWPORT m_viewport;

public:
	DeferredBuffers();
	DeferredBuffers(const DeferredBuffers&);
	~DeferredBuffers();

	bool Initialize(ID3D12Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTargets(ID3D12DeviceContext*);
	void ClearRenderTargets(ID3D12DeviceContext*, float, float, float, float);

	ID3D12ShaderResourceView* GetShaderResourceView(int);
};
#endif