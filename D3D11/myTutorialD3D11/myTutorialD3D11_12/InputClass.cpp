#include "InputClass.h"

InputClass::InputClass(void)
{
}

InputClass::InputClass(const InputClass&)
{

}

InputClass::~InputClass(void)
{
}

void InputClass::Initialize()
{
	//初始所有的键都是非按下状态
	for each (bool& var in m_keys)
		var = false;
}

void InputClass::KeyDown(unsigned int input)
{
	//键被按下
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	//释放按键
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}