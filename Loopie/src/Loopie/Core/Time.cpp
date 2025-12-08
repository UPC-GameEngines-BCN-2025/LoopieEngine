#include "Time.h"
#include <SDL3/SDL_timer.h>
namespace Loopie
{
	float Time::s_LastFrameTime = 0;
	int Time::s_FrameCount = 0;
	float Time::s_FixedDeltaTime = 0.2f;
	float Time::s_TimeScale = 1;
	float Time::s_DeltaTime = 0;
	float Time::s_RunTime = 0;
	float Time::s_ExecutionTime = 0;

	void Time::CalculateFrame()
	{
		Uint64 now = SDL_GetPerformanceCounter();
		if (s_LastFrameTime != 0)
		{
			s_DeltaTime = (float)((now - s_LastFrameTime) / (double)SDL_GetPerformanceFrequency());
		}
		else
		{
			s_DeltaTime = 0.0f;
		}
		s_LastFrameTime = now;

		s_RunTime += GetDeltaTime();
		s_ExecutionTime += GetUnscaledDeltaTime();

		s_FrameCount++;
	}

	void Time::SetFixedDeltaTimeMs(float ms)
	{
		s_FixedDeltaTime = ms * 0.001f;
	}

	void Time::SetFixedDeltaTime(float seconds)
	{
		s_FixedDeltaTime = seconds;
	}

	void Time::SetTimeScale(float scale)
	{
		if (scale < 0.0f)
			scale = 0.0f;

		s_TimeScale = scale;
	}
}