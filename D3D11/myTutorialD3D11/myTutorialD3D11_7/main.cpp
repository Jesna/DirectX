#include "SystemClass.h"

//应用程序入口main函数 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// 创建一个system对象. 
	System = new SystemClass;
	if(!System)
		return 0;

	// 初始化system对象
	result = System->Initialize();
	if(result)
		System->Run();

	// 关闭以及释放system对象
	System->ShutDown();
	delete System;
	System = nullptr;

	return 0;
}