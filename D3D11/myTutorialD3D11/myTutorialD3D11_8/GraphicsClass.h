#pragma once

#include <windows.h>

const bool FULL_SCREEN = false;			//�Ƿ�ȫ��
const bool VSYNC_ENABLED = true;	//�Ƿ�ֱͬ��
const float SCREEN_DEPTH = 1000.0f;	//��ȣ�Զ��
const float SCREEN_NEAR = 1.0f;			//��ȣ�����

class D3DClass;
class CameraClass;
class ModelClass;
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
	CameraClass* m_Camera;		//��Ϊpublic��������SystemClass�п���

private:
	D3DClass* m_D3D;				//����һ��D3DClass���Ա����
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;
};

