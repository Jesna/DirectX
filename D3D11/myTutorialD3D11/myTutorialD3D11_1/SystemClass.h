#pragma once
//����ú��ܹ�����windowsͷ�ļ��Ĵ�С��ʹ������������һЩ����Ҫ���ļ����ӿ����ʱ�� 
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

const bool FULL_SCREEN = false;		//�ж��Ƿ�ȫ����ȫ��ģʽ�ʹ���ģʽ��ʹ�ò�ͬ�Ĵ������ڲ���
static bool bexit = false;					//�����ж��Ƿ�ESC��,����ESC�������˳�����

class SystemClass
{
public:
	SystemClass(void);
	SystemClass(const SystemClass&);
	~SystemClass(void);

	bool Initialize();
	void ShutDown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR	m_applicationName;
	HINSTANCE	m_hinstance;
	HWND m_hwnd;
};

//���徲̬�Ļص������Լ�Ӧ�ó����� 
//��Ϊ���崰�����ʱ�򣬱���ָ�����ڻص����������������þ�̬�Ļص�����WndProc 
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;