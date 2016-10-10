#pragma once

#include <wrl.h>

class Timer
{
private:
	LARGE_INTEGER qpcFrequency;
	LARGE_INTEGER qpcLastTime;
	UINT64 qpcMaxDelta;
	UINT64 totalTicks;
	UINT64 elapsedTicks;

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

	UINT64 GetElapsedTicks()
	{
		return this->elapsedTicks;
	}

	DOUBLE GetElapsedSeconds()
	{
		return (DOUBLE)this->elapsedTicks / this->ticksPerSecond;
	}

	UINT64 GetTotalTicks()
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

		this->elapsedTicks = currentTime.QuadPart - this->qpcLastTime.QuadPart;

		this->totalTicks += this->elapsedTicks;

		this->qpcLastTime = currentTime;
	}
};
