#ifndef _D3D_H_
#define _D3D_H_

//preprocessor links
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//includes
#include <d3d12.h>
#include <dxgi1_4.h>
#include <fstream>

using namespace std;

class D3D{
	bool m_vsync_enabled;
	ID3D12Device* m_device;
	ID3D12CommandQueue* m_commandQueue;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain3* m_swapChain;
	ID3D12DescriptorHeap* m_renderTargetViewHeap;
	ID3D12Resource* m_backBufferRenderTarget[2];
	unsigned int m_bufferIndex;
	ID3D12CommandAllocator* m_commandAllocator;
	ID3D12GraphicsCommandList* m_commandList;
	ID3D12PipelineState* m_pipelineState;
	ID3D12Fence* m_fence;
	HANDLE m_fenceEvent;
	unsigned long long m_fenceValue;
	ID3D12RootSignature* m_rootSignature;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	UINT m_rtvDescriptorSize;

	void GetHardwareAdapter(IDXGIFactory4*, IDXGIAdapter1**);
	void GetVideoCardInfo(char*, int&);

public:
	D3D();
	D3D(const D3D&);
	~D3D();

	bool Initialize(int, int, HWND, bool, bool);
	void Shutdown();

	bool Render();
};
#endif