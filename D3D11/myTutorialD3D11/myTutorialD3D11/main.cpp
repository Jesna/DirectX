#include <windows.h>

LPCWSTR		m_applicationName;		//�����������
HINSTANCE m_hinstance;				//Ӧ�ó���ʵ�����
HWND			m_hwnd;						//���ھ��
bool				bexit = false;					//�����ж��Ƿ�ESC��,����ESC�������˳�����
const bool	FULL_SCREEN = false;	//�ж��Ƿ�ȫ����ȫ��ģʽ�ʹ���ģʽ��ʹ�ò�ͬ�Ĵ������ڲ���

//��ʼ�������࣬����Ӧ�ó��򴰿� 
void InitializeWindows(int& screenWidth, int& screenHeight);
//���ó�ʼ�����ں������Լ������Ľӿ���ĳ�ʼ������ 
bool Initialize();
//������Ϣѭ�� 
void Run();
//�رմ���
void ShutdownWindows();

//���ڻص�����
static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Ӧ�ó������main���� 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Initialize();
	Run();
	ShutdownWindows();
	return 0;
}

//���ô��ڳ�ʼ������������һЩ��ĳ�ʼ������ 
bool Initialize()
{
	int screenWidth = 0, screentHeight = 0;

	// ��ʼ������ 
	InitializeWindows(screenWidth, screentHeight);

	return true;
}

void InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

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

	DWORD err;
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

//������Ϣ 
void Run()
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
			result = bexit;
			if(result)
				done = true;
		}
	}
}

//WndProc����Ϊ���ڻص������������е���Ϣ���������������
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
		return MessageHandler(hwnd, umessage, wparam, lparam);
	}
}

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	// ��ⰴ����Ϣ
	case WM_KEYDOWN:
		if(wparam == VK_ESCAPE)
			bexit = true;
		return 0;
	//�κ�������Ϣ���͵�windowsȱʡ����
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

void ShutdownWindows()
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
}