#include "d3d.h"

D3D::D3D(){
	m_device = 0;
	m_commandQueue = 0;
	m_swapChain = 0;
	m_renderTargetViewHeap = 0;
	m_backBufferRenderTarget[0] = 0;
	m_backBufferRenderTarget[1] = 0;
	m_commandAllocator = 0;
	m_commandList = 0;
	m_pipelineState = 0;
	m_fence = 0;
	m_fenceEvent = 0;
}

D3D::D3D(const D3D& other){}

D3D::~D3D(){}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
void D3D::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
{
	*ppAdapter = nullptr;
	for (UINT adapterIndex = 0;; ++adapterIndex)
	{
		IDXGIAdapter1* pAdapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
		{
			// No more adapters to enumerate.
			break;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_1, _uuidof(ID3D12Device), nullptr)))
		{
			*ppAdapter = pAdapter;
			return;
		}
		pAdapter->Release();
	}
}

bool D3D::Initialize(int screenHeight, int screenWidth, HWND hwnd, bool vsync, bool fullscreen){
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT result;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	IDXGIFactory4* factory;
	IDXGIAdapter* adapter;
	IDXGIAdapter1* pAdapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, renderTargetViewDescSize;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	IDXGISwapChain* swapChain;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;

	//set vsync
	m_vsync_enabled = vsync;

	//set feature level
	featureLevel = D3D_FEATURE_LEVEL_11_1;

	//create a DirectX graphics interface factory
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
	if (FAILED(result)) return false;

	//get hardware adapter
	GetHardwareAdapter(factory, &pAdapter);

	//create d3d12 device
	result = D3D12CreateDevice(pAdapter, featureLevel, __uuidof(ID3D12Device), (void**)&m_device);
	if (FAILED(result)){
		MessageBox(hwnd, L"Could not create DirectX device. The default card does not support DirectX feature levels 11.1 or higher.",
			L"DirectX Device Failure", MB_OK);
		return false;
	}

	//initialise command queue descriptor
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	//set command queue description
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	//create the command queue
	result = m_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_commandQueue);
	if (FAILED(result)) return false;

	//create a DirectX graphics interface factory
	//result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
	//if (FAILED(result)) return false;

	//user the factory to create video card adapter
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) return false;

	//enumerate primary adapter output (monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) return false;

	//get no. of modes that fit DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) return false;

	//create a list holding all possible display modes for HW devices
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) return false;

	//fill display mode list
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) return false;

	for (i = 0; i < numModes; i++){
		if (displayModeList[i].Height = (unsigned int)screenHeight){
			if (displayModeList[i].Width = (unsigned int)screenWidth){
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//get adapter (video card) desciption
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) return false;

	//store dedicated video card memory in Mb
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//store name of video card
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) return false;

	//output video card info
	GetVideoCardInfo(m_videoCardDescription, m_videoCardMemory);

	//release displayModeList
	delete[] displayModeList;
	displayModeList = 0;

	//release adapter output
	adapterOutput->Release();
	adapterOutput = 0;

	//release adapter
	adapter->Release();
	adapter = 0;

	//swap chain
	//initialise swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//set double buffering
	swapChainDesc.BufferCount = 2;

	//set height and width of swap chain back buffers
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Width = screenWidth;

	//set a regular 32-bit surface for back buffers
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//set back buffers to be render target outputs
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//set discard previous buffer contents after swap
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//set window handle for rendering
	swapChainDesc.OutputWindow = hwnd;

	//set fullscreen/windowed
	if (fullscreen) swapChainDesc.Windowed = false;
	else swapChainDesc.Windowed = true;

	//set back buffer refresh rate
	if (m_vsync_enabled){
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//turn off multisampling
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//set unspecified for scanline ordering/scaling
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//neglect advanced flags
	swapChainDesc.Flags = 0;

	//create swap chain using description
	result = factory->CreateSwapChain(m_commandQueue, &swapChainDesc, &swapChain);
	if (FAILED(result)) return false;

	//upgrade member variable IDXGISwapChain to IDXGISwapChain3 interface
	//more functionality (back buffer index access)
	result = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_swapChain);
	if (FAILED(result)) return false;

	//clear pointer to original swap chain interface (m_swapChain is the v3 interface)
	swapChain = 0;

	//release factory since swapchain made
	factory->Release();
	factory = 0;

	//initalise render target view heap desc
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	//set no of desc to 2 for our 2 back buffers. set heap type to rendertargetview
	renderTargetViewHeapDesc.NumDescriptors = 2;
	renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	//create rtv heap for back buffers
	result = m_device->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_renderTargetViewHeap);
	if (FAILED(result)) return false;

	//get handle to starting memory
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

	//get size of memory location for the render target view desc
	renderTargetViewDescSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//get pointer to 1st back buffer from swapchain
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&m_backBufferRenderTarget[0]);
	if (FAILED(result)) return false;

	//create rtv for 1st back buffer
	m_device->CreateRenderTargetView(m_backBufferRenderTarget[0], NULL, renderTargetViewHandle);

	//increment view handle to next description
	renderTargetViewHandle.ptr += renderTargetViewDescSize;

	//get pointer to 2nd back buffer from swapchain
	result = m_swapChain->GetBuffer(1, __uuidof(ID3D12Resource), (void**)&m_backBufferRenderTarget[1]);
	if (FAILED(result)) return false;

	//create rtv for 2nd back buffer
	m_device->CreateRenderTargetView(m_backBufferRenderTarget[1], NULL, renderTargetViewHandle);

	//get initial index to current back buffer
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	//create command allocator (free mem for each frame)
	result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_commandAllocator);
	if (FAILED(result)) return false;
	
	//INITIALIZE THE ASSETS TO BE DONE IN OTHER FILE
	//create an empty root signature

	//compile the shaders

	//create a pipeline state object description, then create the object


	//create basic command list
	//command list contains render commands for each frame (can be more lists for parallel execution)
	result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_commandList);
	if (FAILED(result)) return false;

	//close command list for init to avoid recording pre-steps
	result = m_commandList->Close();
	if (FAILED(result)) return false;

	//create fence for GPU to notify complete rendering of list
	result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_fence);
	if (FAILED(result)) return false;

	//create event object for fence
	m_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_fenceEvent == NULL) return false;

	//init starting fence value
	m_fenceValue = 1;

	return true;
}

void D3D::Shutdown(){
	int error;

	//change to windowed mode to avoid exception
	if (m_swapChain) m_swapChain->SetFullscreenState(false, NULL);

	//close object handle to fence
	error = CloseHandle(m_fenceEvent);
	if (error == 0){}

	//release fence
	if (m_fence){
		m_fence->Release();
		m_fence = 0;
	}

	//release empty pipeline state
	if (m_pipelineState){
		m_pipelineState->Release();
		m_pipelineState = 0;
	}

	// Release the command list.
	if (m_commandList){
		m_commandList->Release();
		m_commandList = 0;
	}

	// Release the command allocator.
	if (m_commandAllocator){
		m_commandAllocator->Release();
		m_commandAllocator = 0;
	}

	// Release the back buffer render target views.
	if (m_backBufferRenderTarget[0]){
		m_backBufferRenderTarget[0]->Release();
		m_backBufferRenderTarget[0] = 0;
	}
	if (m_backBufferRenderTarget[1]){
		m_backBufferRenderTarget[1]->Release();
		m_backBufferRenderTarget[1] = 0;
	}

	// Release the render target view heap.
	if (m_renderTargetViewHeap){
		m_renderTargetViewHeap->Release();
		m_renderTargetViewHeap = 0;
	}

	// Release the swap chain.
	if (m_swapChain){
		m_swapChain->Release();
		m_swapChain = 0;
	}

	// Release the command queue.
	if (m_commandQueue){
		m_commandQueue->Release();
		m_commandQueue = 0;
	}

	// Release the device.
	if (m_device){
		m_device->Release();
		m_device = 0;
	}
}

bool D3D::Render(){
	HRESULT result;
	D3D12_RESOURCE_BARRIER barrier;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	unsigned int renderTargetViewDescSize;
	ID3D12CommandList* ppComandLists[1];
	unsigned long long fenceToWaitFor;

	//reset memory associated command allocator
	result = m_commandAllocator->Reset();
	if (FAILED(result)) return false;

	//rest command list, use empty pipeline state as no shaders present atm
	result = m_commandList->Reset(m_commandAllocator, m_pipelineState);
	if (FAILED(result)) return false;

	//record commands into command list
	//set resource barrier for syncing next back buffer
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_backBufferRenderTarget[m_bufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_commandList->ResourceBarrier(1, &barrier);

	//get render target view handle for current back buffer
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (m_bufferIndex == 1){
		renderTargetViewHandle.ptr += renderTargetViewDescSize;
	}

	//set back buffer as render target
	m_commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, NULL);

	//set clear colour
	const float clearColour[] = { 0.05f, 0.0f, 0.1f, 1.0f };

	m_commandList->ClearRenderTargetView(renderTargetViewHandle, clearColour, 0, NULL);

	//more commands
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//indicate back buffer is to be used to present
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_commandList->ResourceBarrier(1, &barrier);

	//close command list
	result = m_commandList->Close();
	if (FAILED(result)) return false;

	//load commandlist array (only 1 item atm)
	ppComandLists[0] = m_commandList;

	//execute command lists
	m_commandQueue->ExecuteCommandLists(1, ppComandLists);

	//present back buffer to screen as render is complete
	if (m_vsync_enabled){
		//lock to refresh rate
		result = m_swapChain->Present(1, 0);
		if (FAILED(result)) return false;
	}
	else{
		//present asap
		result = m_swapChain->Present(0, 0);
		if (FAILED(result)) return false;
	}

	//signal and inc fence value
	fenceToWaitFor = m_fenceValue;
	result = m_commandQueue->Signal(m_fence, fenceToWaitFor);
	if (FAILED(result)) return false;
	m_fenceValue++;

	//wait until GPU is done rendering
	if (m_fence->GetCompletedValue() < fenceToWaitFor){
		result = m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
		if (FAILED(result)) return false;
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
	
	//alternate the back buffer index between frame
	m_bufferIndex == 0 ? m_bufferIndex = 1 : m_bufferIndex = 0;

	return true;
}

//output video card info to file
void D3D::GetVideoCardInfo(char* cardName, int& memory){
	ofstream fout;
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	fout.open("video-card-info.txt");
	for (int i = 0; i < 128; i++){
		if (cardName[i] == '\0') break;
		fout << cardName[i];
	}
	fout << " with ";
	fout << memory;
	fout << " Mb dedicated video memory";
	fclose;
}