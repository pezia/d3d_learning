#pragma once

#include <wrl.h>

class Timer
{
private:
	LARGE_INTEGER qpcFrequency;
	LARGE_INTEGER qpcLastTime;
	INT64 elapsedTicks;
	INT64 totalTicks;

public:
	static const UINT64 ticksPerSecond = 10000000;

	Timer()
	{
		QueryPerformanceFrequency(&this->qpcFrequency);
		QueryPerformanceCounter(&this->qpcLastTime);
	}

	~Timer()
	{
	}

	INT64 GetElapsedTicks()
	{
		return this->elapsedTicks;
	}

	DOUBLE GetElapsedSeconds()
	{
		return (DOUBLE)this->elapsedTicks / this->ticksPerSecond;
	}

	INT64 GetTotalTicks()
	{
		return this->totalTicks;
	}

	DOUBLE GetTotalSeconds()
	{
		return (DOUBLE)this->totalTicks / this->ticksPerSecond;
	}

	void Tick()
	{
		LARGE_INTEGER currentTime;

		QueryPerformanceCounter(&currentTime);

		UINT64 delta = currentTime.QuadPart - this->qpcLastTime.QuadPart;

		this->qpcLastTime = currentTime;
	}
};
