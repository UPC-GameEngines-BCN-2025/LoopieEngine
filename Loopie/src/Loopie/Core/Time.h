namespace Loopie {
	class Time
	{
	public:

		Time() = delete;
		~Time() = delete;

		static void CalculateFrame();

		static void SetFixedDeltaTime(float seconds);
		static void SetFixedDeltaTimeMs(float ms);
		static void SetTimeScale(float scale);

		static float GetDeltaTime() { return s_DeltaTime * s_TimeScale; }
		static float GetDeltaTimeMs() { return s_DeltaTime * 1000.f * s_TimeScale; }

		static float GetFixedDeltaTime() { return s_FixedDeltaTime * s_TimeScale; }
		static float GetFixedDeltaTimeMs() { return s_FixedDeltaTime * 1000.f * s_TimeScale; }

		static float GetUnscaledDeltaTime() { return s_DeltaTime; }
		static float GetUnscaledDeltaTimeMs() { return s_DeltaTime * 1000.f; }

		static float GetUnscaledFixedDeltaTime() { return s_FixedDeltaTime; }
		static float GetUnscaledFixedDeltaTimeMs() { return s_FixedDeltaTime * 1000.f; }

		static float GetLastFrameTime() { return s_LastFrameTime; }

		static float GetRunTime() { return s_RunTime; }
		static float GetRunTimeMs() { return s_RunTime * 1000.f; }

		static float GetExecutionTime() { return s_ExecutionTime; }
		static float GetExecutionTimeMs() { return s_ExecutionTime * 1000.f; }

		static int GetFrameCount() { return s_FrameCount; }
		static float GetTimeScale() { return s_TimeScale; }

	private:
		static float s_LastFrameTime;
		static int s_FrameCount;

		//Physic
		static float s_FixedDeltaTime;

		// Game Clock
		static float s_DeltaTime;
		static float s_TimeScale;
		static float s_RunTime;

		//Real Time Clock
		static float s_ExecutionTime;
	};
}