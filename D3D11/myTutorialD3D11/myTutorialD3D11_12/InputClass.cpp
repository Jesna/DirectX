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
	//��ʼ���еļ����Ƿǰ���״̬
	for each (bool& var in m_keys)
		var = false;
}

void InputClass::KeyDown(unsigned int input)
{
	//��������
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	//�ͷŰ���
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}