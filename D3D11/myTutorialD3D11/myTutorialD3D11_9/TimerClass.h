#pragma once

#include <windows.h>

//主要用来计算相邻两帧之间的时间 
//可以用来实现平滑的与fps无关的动画。
class TimerClass
{
public:
	TimerClass(void);
	TimerClass(const TimerClass&);
	~TimerClass(void);

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64	m_frequency;
	float		m_ticksPerMs;
	INT64	m_startTime;
	float		m_frameTime;
};

