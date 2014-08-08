#include "SystemClass.h"
#include "InputClass.h"
#include "GraphicsClass.h"
#include "CameraClass.h"
#include "TimerClass.h"

SystemClass::SystemClass(void)
{
	m_Input = nullptr;
	m_Graphics = nullptr;
	m_Timer = nullptr;
}

SystemClass::SystemClass(const SystemClass&)
{

}

SystemClass::~SystemClass(void)
{
}

//���ô��ڳ�ʼ������������һЩ��ĳ�ʼ������ 
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;

	// ��ʼ������ 
	InitializeWindows(screenWidth, screenHeight);

	//����input������������� 
	m_Input = new InputClass;
	if(!m_Input)
		return false;

	// ��ʼ���������
	m_Input->Initialize();

	// ����ͼ�ζ������������ȾӦ�ó����е��������� 
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
		return false;

	// ��ʼ��ͼ�ζ��� 
	bool result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
		return false;

	//������ʱ������
	m_Timer = new TimerClass;
	if(!m_Timer)
		return false;

	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(m_hwnd, TEXT("Could not initialize the Timer object."), TEXT("Error"), MB_OK); 
		return false; 
	}

	return true;
}

void SystemClass::ShutDown()
{
	// �ͷż�ʱ������
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}

	if(m_Graphics)
	{
		m_Graphics->ShutDown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	if(m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	// ִ�д���һЩ���ٹ���
	ShutdownWindows();
}

//������Ϣ 
void SystemClass::Run()
{
	MSG msg;
	bool done = false, result = true;

	// ��ʼ����Ϣ�ṹ
	ZeroMemory(&msg, sizeof(MSG));

	// ѭ��������Ϣ����
	while (!done)
	{
		// ����windows��Ϣ
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ���յ�WM_QUIT��Ϣ���˳�����
		if(msg.message == WM_QUIT)
			done = true;
		else
		{
			//�������ESC,Ҳ�˳�����
			result = Frame();
			if(!result)
				done = true;
		}
	}
}

bool SystemClass::Frame()
{
	//����û��Ƿ���ESC����������£��˳�����
	if(m_Input->IsKeyDown(VK_ESCAPE))
		return false;

	//���A,S,D,W,Q,E,Z,X,C�����£��ƶ������
	if(GetAsyncKeyState('W') & 0x8000)			//ǰ��
		m_Graphics->m_Camera->walk(-0.1f);
	if(GetAsyncKeyState('S') & 0x8000)
		m_Graphics->m_Camera->walk(0.1f);
	if(GetAsyncKeyState('A') & 0x8000)			//����
		m_Graphics->m_Camera->strafe(-0.1f);
	if(GetAsyncKeyState('D') & 0x8000)
		m_Graphics->m_Camera->strafe(0.1f);
	if(GetAsyncKeyState('Q') & 0x8000)			//����
		m_Graphics->m_Camera->fly(-0.1f);
	if(GetAsyncKeyState('E') & 0x8000)
		m_Graphics->m_Camera->fly(0.1f);
	if(GetAsyncKeyState('Z') & 0x8000)
		m_Graphics->m_Camera->pitch(D3DX_PI / 180);
	if(GetAsyncKeyState('X') & 0x8000)
		m_Graphics->m_Camera->yaw(D3DX_PI / 180);
	if(GetAsyncKeyState('C') & 0x8000)
		m_Graphics->m_Camera->roll(D3DX_PI / 180);
	if(GetAsyncKeyState('R') & 0x8000)			//�������λ
	{
		// ���������λ��
		m_Graphics->m_Camera->Reset();
		D3DXVECTOR3 campos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
		m_Graphics->m_Camera->setPosition(&campos);
	}

	m_Timer->Frame();

	//��������ת�����
	//m_Graphics->m_Camera->roll(m_Timer->GetTime() / 1000);

	// ִ��֡��Ⱦ����
	bool result = m_Graphics->Frame(m_Timer->GetTime() / 1000);
	if(!result)
		return false;

	return true;
}

//��ʼ�������࣬����Ӧ�ó��򴰿� 
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//��ȡSystem class����
	ApplicationHandle = this;
	// �õ�Ӧ�ó���ʵ����� 
	m_hinstance = GetModuleHandle(nullptr);
	// Ӧ�ó������� 
	m_applicationName = TEXT("Engine");

	// ���ô��������.
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

	// ע�ᴰ���� 
	RegisterClassEx(&wc);

	// �õ�windows����ֱ��� 
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// �����Ƿ�ȫ�����ò�ͬ�ķֱ���
	if(FULL_SCREEN)
	{
		//ȫ��ģʽ�£����ô��ڴ�СΪwindows����ֱ���
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (DWORD)screenWidth;
		dmScreenSettings.dmPelsHeight = (DWORD)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ��ʱ������ʾ�豸Ϊȫ��ģʽ��ע�⣺Ӧ�ó����˳�ʱ�򣬽��ָ�ϵͳĬ������
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ���ô��ڵ����Ͻ�����λ��Ϊ(0,0)
		posX = posY = 0;
	}
	else
	{
		// ����ģʽ��800*600
		screenWidth = 800;
		screenHeight = 600;

		// �������Ͻ�����λ��,posX, posY
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ȫ���ʹ���ʹ�ò�ͬ�Ĳ���. 
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

	// ��ʾ���ڲ�������Ϊ����. 
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//�������
	ShowCursor(FALSE);
}

void SystemClass::ShutdownWindows()
{
	//��ʾ���
	ShowCursor(true);

	// �ָ�Ĭ����ʾ����
	if(FULL_SCREEN)
		ChangeDisplaySettings(nullptr, 0);

	//�ͷŴ��ھ��
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// �ͷ�Ӧ�ó���ʵ��
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;
	ApplicationHandle = nullptr;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// ��ⰴ����Ϣ
	case WM_KEYDOWN:
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	case WM_KEYUP:
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
		//�κ�������Ϣ���͵�windowsȱʡ����
	case WM_SIZE:
		{
			int screenWidth = 0, screenHeight = 0;
			screenWidth = LOWORD(lparam);
			screenHeight = HIWORD(lparam);

			// ���ڴ�С�ı�ʱ�����³�ʼ��ͼ�ζ���
			if(m_Graphics)
			{
				bool result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
				if(!result)
					return 0;
			}

			return 0;
		}
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