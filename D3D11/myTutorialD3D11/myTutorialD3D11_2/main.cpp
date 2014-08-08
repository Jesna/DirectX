#include "SystemClass.h"

//Ӧ�ó������main���� 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// ����һ��system����. 
	System = new SystemClass;
	if(!System)
		return 0;

	// ��ʼ��system����
	result = System->Initialize();
	if(result)
		System->Run();

	// �ر��Լ��ͷ�system����
	System->ShutDown();
	delete System;
	System = nullptr;

	return 0;
}