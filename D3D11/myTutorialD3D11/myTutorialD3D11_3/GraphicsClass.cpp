#include "GraphicsClass.h"
#include "D3DClass.h"

GraphicsClass::GraphicsClass(void)
{
	m_D3D = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{

}

GraphicsClass::~GraphicsClass(void)
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// ����һ��D3DClass����
	m_D3D = new D3DClass;
	if(!m_D3D)
		return false;

	// ����D3DClass��ʼ������ 
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, 
		SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, TEXT("Could not initialize Direct3D"), TEXT("Error"), MB_OK); 
		return false; 
	}

	return true;
}

void GraphicsClass::ShutDown()
{
	//����m_D3D���� 
	if(m_D3D)
	{
		m_D3D->ShutDown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	//����Render��������Ⱦ3D����
	bool result = Render();
	if(!result)
		return false;

	return true;
}

bool GraphicsClass::Render()
{
	// ����framebuffer.Ϊǳ��ɫ
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);

	//��framebuffer�е�ͼ��present����Ļ��
	m_D3D->EndScene();

	return true;
}