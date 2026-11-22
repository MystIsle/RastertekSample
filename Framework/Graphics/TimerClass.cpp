#include "TimerClass.h"

TimerClass::TimerClass()
{
	
}

TimerClass::TimerClass(const TimerClass&)
{
	
}

TimerClass::~TimerClass()
{
	
}

bool TimerClass::Initialize()
{
	INT64 frequency;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	if (frequency == 0)
	{
		return false;
	}
	
	m_frequency = (float)frequency;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	INT64 elapedTicks;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	elapedTicks = currentTime - m_startTime;
	m_frameTime = (float)elapedTicks / m_frequency;
	m_startTime = currentTime;
}

float TimerClass::GetTime()
{
	return m_frameTime;
}
