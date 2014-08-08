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

	// 创建一个D3DClass对象
	m_D3D = new D3DClass;
	if(!m_D3D)
		return false;

	// 调用D3DClass初始化函数 
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
	//销毁m_D3D对象 
	if(m_D3D)
	{
		m_D3D->ShutDown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	//调用Render函数，渲染3D场景
	bool result = Render();
	if(!result)
		return false;

	return true;
}

bool GraphicsClass::Render()
{
	// 设置framebuffer.为浅蓝色
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);

	//把framebuffer中的图像present到屏幕上
	m_D3D->EndScene();

	return true;
}