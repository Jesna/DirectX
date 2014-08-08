#pragma once

#include <windows.h>

const bool FULL_SCREEN = false;			//是否全屏
const bool VSYNC_ENABLED = true;	//是否垂直同步
const float SCREEN_DEPTH = 1000.0f;	//深度，远点
const float SCREEN_NEAR = 1.0f;			//深度，近点

class D3DClass;
class CameraClass;
class ModelClass;
class AxisModelClass;
class ColorShaderClass;

class GraphicsClass
{
public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);

	bool Initialize(int, int, HWND);
	void ShutDown();
	bool Frame();

private:
	bool Render();

public:
	CameraClass* m_Camera;		//设为public，便于在SystemClass中控制

private:
	D3DClass* m_D3D;				//定义一个D3DClass类成员变量
	ModelClass* m_Model;
	AxisModelClass* m_AxisModel;
	ColorShaderClass* m_ColorShader;
};

