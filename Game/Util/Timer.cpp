#include "Timer.h"

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

Timer::Timer(float targetTime)
	: targetTime(targetTime)
{
}

void Timer::Tick(float deltaTime)
{
	// 경과 시간 업데이트.
	elapsedTime += deltaTime;
}

void Timer::Reset()
{
	elapsedTime = 0.0f;
}

bool Timer::IsTimeOut() const
{
	return elapsedTime >= targetTime;
}

void Timer::SetTargetTime(float newTargetTime)
{
	targetTime = newTargetTime;
}

