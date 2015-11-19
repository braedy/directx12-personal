#ifndef _APPLICATION_H_
#define _APPLICATION_H_

//includes
#include <Windows.h>

//includes
#include "input.h"
#include "d3d.h"
//#include "light.h"
//#include "camera.h"
//#include "model.h"
//#include "orthowindow.h"
//#include "deferredbuffers.h"
//#include "deferredshader.h"
//#include "deferredlightshader.h"
//#include "fps.h"
//#include "cpu.h"
//#include "timer.h"
//#include "position.h"

//globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.f;
const float SCREEN_NEAR = 1.0f;

class Application{
	Input* m_Input;
	D3D* m_D3D;

	/*Camera* m_Camera;
	Light* m_Light;
	Model* m_Model;

	OrthoWindow* m_FullScreenWindow;
	DeferredBuffers* m_DeferredBuffers;
	DeferredShader* m_DeferredShader;
	DeferredLightShader* m_DeferredLightShader;

	Fps* m_Fps;
	Cpu* m_Cpu;
	Timer* m_Timer;
	Position* m_Position;*/

	bool Render();
	bool RenderSceneToTexture();

public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
};
#endif