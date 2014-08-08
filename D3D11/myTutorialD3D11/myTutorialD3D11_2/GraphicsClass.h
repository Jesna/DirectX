#pragma once

#include <windows.h>

const bool FULL_SCREEN = false;			//�Ƿ�ȫ��
const bool VSYNC_ENABLED = true;	//�Ƿ�ֱͬ��
const float SCREEN_DEPTH = 1000.0f;	//��ȣ�Զ��
const float SCREEN_NEAR = 1.0f;			//��ȣ�����

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
};

