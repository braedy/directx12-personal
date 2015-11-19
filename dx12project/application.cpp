#include "application.h"


Application::Application()
{
	m_Input = 0;
	m_D3D = 0;

	/*m_Camera = 0;
	m_Light = 0;
	m_Model = 0;

	m_FullScreenWindow = 0;
	m_DeferredBuffers = 0;
	m_DeferredShader = 0;
	m_DeferredLightShader = 0;

	m_Cpu = 0;
	m_Fps = 0;
	m_Timer = 0;
	m_Position = 0;*/
}

Application::Application(const Application& other)
{
}


Application::~Application()
{
}


bool Application::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight){
	bool result;

	// Create the Direct3D object.
	m_D3D = new D3D;
	if (!m_D3D)	return false;

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN); //, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result){
		MessageBox(hwnd, L"Could not initialize DirectX.", L"Error", MB_OK);
		return false;
	}

	// create input object
	m_Input = new Input;
	if (!m_Input) return false;

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	//// create fps object for frame rate checking
	//m_Fps = new Fps;
	//if (!m_Fps) return false;
	//// initialize fps object
	//m_Fps->Initialize();

	//// create cpu object for process checking
	//m_Cpu = new Cpu;
	//if (!m_Cpu) return false;
	//// initialize cpu object
	//m_Cpu->Initialize();

	//// create timer object
	//m_Timer = new Timer;
	//if (!m_Timer) return false;
	//// initialize timer object
	//result = m_Timer->Initialize();
	//if (!result){
	//	MessageBox(hwnd, "Could not initialize the Timer object.", "Error", MB_OK);
	//	return false;
	//}

	////create position object
	//m_Position = new Position;
	//if (!m_Position) return false;

	//// Create the camera object.
	//m_Camera = new Camera;
	//if (!m_Camera) return false;
	//XMMATRIX baseViewMatrix; //temp

	//// Set the initial position of the camera and build the matrices needed for rendering.
	//m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	//m_Camera->Render();
	//m_Camera->RenderBaseViewMatrix();

	//// Create the light object.
	//m_Light = new Light;
	//if (!m_Light) return false;

	//// Initialize the light object.
	//m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	//// Create the model object.
	//m_Model = new Model;
	//if (!m_Model) return false;

	//// Initialize the model object.
	//result = m_Model->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "cube.txt", "lava.tga");
	//if (!result)
	//{
	//	MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
	//	return false;
	//}

	//// Create the full screen ortho window object.
	//m_FullScreenWindow = new OrthoWindow;
	//if (!m_FullScreenWindow) return false;

	//// Initialize the full screen ortho window object.
	//result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	//if (!result){
	//	MessageBox(hwnd, "Could not initialize the full screen ortho window object.", "Error", MB_OK);
	//	return false;
	//}

	//// Create the deferred buffers object.
	//m_DeferredBuffers = new DeferredBuffers;
	//if (!m_DeferredBuffers){
	//	return false;
	//}

	//// Initialize the deferred buffers object.
	//result = m_DeferredBuffers->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	//if (!result){
	//	MessageBox(hwnd, "Could not initialize the deferred buffers object.", "Error", MB_OK);
	//	return false;
	//}

	//// Create the deferred shader object.
	//m_DeferredShader = new DeferredShader;
	//if (!m_DeferredShader){
	//	return false;
	//}

	//// Initialize the deferred shader object.
	//result = m_DeferredShader->Initialize(m_D3D->GetDevice(), hwnd);
	//if (!result){
	//	MessageBox(hwnd, "Could not initialize the deferred shader object.", "Error", MB_OK);
	//	return false;
	//}

	//// Create the light shader object.
	//m_DeferredLightShader = new DeferredLightShader;
	//if (!m_DeferredLightShader)	return false;

	//// Initialize the light shader object.
	//result = m_DeferredLightShader->Initialize(m_D3D->GetDevice(), hwnd);
	//if (!result){
	//	MessageBox(hwnd, "Could not initialize the light shader object.", "Error", MB_OK);
	//	return false;
	//}

	return true;
}

void Application::Shutdown()
{
	//// Release the light shader object.
	//if (m_DeferredLightShader)
	//{
	//	m_DeferredLightShader->Shutdown();
	//	delete m_DeferredLightShader;
	//	m_DeferredLightShader = 0;
	//}

	//// Release the deferred shader object.
	//if (m_DeferredShader){
	//	m_DeferredShader->Shutdown();
	//	delete m_DeferredShader;
	//	m_DeferredShader = 0;
	//}

	//// Release the deferred buffers object.
	//if (m_DeferredBuffers){
	//	m_DeferredBuffers->Shutdown();
	//	delete m_DeferredBuffers;
	//	m_DeferredBuffers = 0;
	//}

	//// Release the full screen ortho window object.
	//if (m_FullScreenWindow){
	//	m_FullScreenWindow->Shutdown();
	//	delete m_FullScreenWindow;
	//	m_FullScreenWindow = 0;
	//}

	//// Release the model object.
	//if (m_Model){
	//	m_Model->Shutdown();
	//	delete m_Model;
	//	m_Model = 0;
	//}

	//// Release the light object.
	//if (m_Light){
	//	delete m_Light;
	//	m_Light = 0;
	//}

	//// Release the camera object.
	//if (m_Camera){
	//	delete m_Camera;
	//	m_Camera = 0;
	//}

	// Release the D3D object.
	if (m_D3D){
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the input object.
	if (m_Input){
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
}


bool Application::Frame(){
	bool result;

	// Read the user input.
	result = m_Input->Frame();
	if (!result) return false;

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed()) return false;

	// Render the graphics.
	result = Render();
	if (!result) return false;

	return true;
}

bool Application::Render(){
	bool result;
	result = m_D3D->Render();
	if (!result) return false;
	//XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	//// Render the scene to the render buffers.
	//result = RenderSceneToTexture();
	//if (!result) return false;

	//// Clear the scene.
	//m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//// Get the matrices.
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_Camera->GetBaseViewMatrix(baseViewMatrix);
	//m_D3D->GetOrthoMatrix(orthoMatrix);

	//// Turn off the Z buffer to begin all 2D rendering.
	//m_D3D->TurnZBufferOff();

	//// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_FullScreenWindow->Render(m_D3D->GetDeviceContext());

	////Now that the scene was rendered with the deferred shader 
	////we can use the render to texture buffers to render the 
	////directional light.We use the re - written light shader as a 
	////2D post processing operation.

	//// Render the full screen ortho window using the deferred light shader and the render buffers.
	//m_DeferredLightShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
	//	m_DeferredBuffers->GetShaderResourceView(0), m_DeferredBuffers->GetShaderResourceView(1),
	//	m_Light->GetDirection());

	//// Turn the Z buffer back on now that all 2D rendering has completed.
	//m_D3D->TurnZBufferOn();

	//// Present the rendered scene to the screen.
	//m_D3D->EndScene();

	return true;
}

bool Application::RenderSceneToTexture(){
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Set the render buffers to be the render target.
	//m_DeferredBuffers->SetRenderTargets(m_D3D->GetDeviceContext());

	//// Clear the render buffers.
	//m_DeferredBuffers->ClearRenderTargets(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//// Get the matrices from the camera and d3d objects.
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_Camera->GetViewMatrix(viewMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);

	//// Update the rotation variable each frame.
	//static float rotation = 0.0f;
	//rotation += (float)XM_PI * 0.01f;
	//if (rotation > 360.0f) rotation -= 360.0f;

	//// Rotate the world matrix by the rotation value so that the cube will spin.
	//worldMatrix = XMMatrixRotationY(rotation);

	//// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_Model->Render(m_D3D->GetDeviceContext());

	//// Render the model using the deferred shader.
	//m_DeferredShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

	////reset render target back to the original back buffer (no longer render buffers)
	//m_D3D->SetBackBufferRenderTarget();

	////reset the viewport back to the original
	//m_D3D->ResetViewport();

	return true;
}