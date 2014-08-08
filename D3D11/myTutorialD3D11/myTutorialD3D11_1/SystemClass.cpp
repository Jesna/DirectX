#include "SystemClass.h"

SystemClass::SystemClass(void)
{
	bexit = false;
}

SystemClass::SystemClass(const SystemClass&)
{

}

SystemClass::~SystemClass(void)
{
}

//调用窗口初始化函数和其它一些类的初始化函数 
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;

	 // 初始化窗口 
	InitializeWindows(screenWidth, screenHeight);

	return true;
}

void SystemClass::ShutDown()
{
	// 执行窗口一些销毁工作
	ShutdownWindows();
}

//处理消息 
void SystemClass::Run()
{
	MSG msg;
	bool done = false, result = true;

	// 初始化消息结构
	ZeroMemory(&msg, sizeof(MSG));

	// 循环进行消息处理
	while (!done)
	{
		// 处理windows消息
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 接收到WM_QUIT消息，退出程序
		if(msg.message == WM_QUIT)
			done = true;
		else
		{
			//如果按了ESC,也退出程序
			result = bexit;
			Frame();
			if(result)
				done = true;
		}
	}
}

bool SystemClass::Frame()
{
	return true;
}

//初始化窗口类，创建应用程序窗口 
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//获取System class对象
	ApplicationHandle = this;
	// 得到应用程序实例句柄 
	m_hinstance = GetModuleHandle(nullptr);
	// 应用程序名字 
	m_applicationName = TEXT("Engine");

	// 设置窗口类参数.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 注册窗口类 
	RegisterClassEx(&wc);

	// 得到windows桌面分辨率 
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 根据是否全屏设置不同的分辨率
	if(FULL_SCREEN)
	{
		//全屏模式下，设置窗口大小为windows桌面分辨率
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (DWORD)screenWidth;
		dmScreenSettings.dmPelsHeight = (DWORD)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 临时设置显示设备为全屏模式，注意：应用程序退出时候，将恢复系统默认设置
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 设置窗口的左上角坐标位置为(0,0)
		posX = posY = 0;
	}
	else
	{
		// 窗口模式：800*600
		screenWidth = 800;
		screenHeight = 600;

		// 窗口左上角坐标位置,posX, posY
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 全屏和窗口使用不同的参数. 
	if(FULL_SCREEN)
	{
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, 
			screenWidth, screenHeight, nullptr, nullptr, m_hinstance, nullptr);
	}
	else
	{
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
			WS_OVERLAPPEDWINDOW, posX, posY, screenWidth, screenHeight, 
			nullptr, nullptr, m_hinstance, nullptr);
	}

	// 显示窗口并设置其为焦点. 
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//隐藏鼠标
	ShowCursor(FALSE);
}

void SystemClass::ShutdownWindows()
{
	//显示光标
	ShowCursor(true);

	// 恢复默认显示设置
	if(FULL_SCREEN)
		ChangeDisplaySettings(nullptr, 0);

	//释放窗口句柄
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// 释放应用程序实例
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;
	ApplicationHandle = nullptr;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 检测按键消息
	case WM_KEYDOWN:
		if(wparam == VK_ESCAPE)
			bexit = true;
		return 0;
		//任何其它消息发送到windows缺省处理
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}