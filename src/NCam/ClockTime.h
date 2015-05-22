#ifndef __CLOCK_TIME_H__
#define __CLOCK_TIME_H__

#include <iostream>
#include <Windows.h>


namespace clk
{
	typedef __int64 time;

	static time GetTime()
	{
		LARGE_INTEGER now;
		static LARGE_INTEGER frequency;
		static int gotfrequency = 0;
		time seconds, nsec;

		QueryPerformanceCounter(&now);
		if (gotfrequency == 0) 
		{
			QueryPerformanceFrequency(&frequency);
			gotfrequency = 1;
		}

		seconds = now.QuadPart / frequency.QuadPart;
		nsec = (1000000000I64 * (now.QuadPart - (seconds * frequency.QuadPart))) / frequency.QuadPart;

		return seconds * 1000000000I64 + nsec;
	}

	static time GetInterval(const time& start, const time& end)
	{
		return end - start;
	}
	
	static float GetIntervalMsec(const time& start, const time& end)
	{
		return (float) (end - start) * 0.000001f;
	}
	
	static float GetIntervalSec(const time& start, const time& end)
	{
		return (float) (end - start) * 0.000000001f;
	}
	
	static float Msec(const time& t)
	{
		return t * 0.000000001f;
	}
	
	static float Sec(const time& t)
	{
		return t * 0.000000001f;
	}
	
	struct Time
	{
		time start;
		time stop;

		void Start()
		{
			start = GetTime();
		}

		void Stop()
		{
			stop = GetTime();
		}

		float GetIntervalMsec()
		{
			return clk::GetIntervalMsec(start, stop);
		}

		float GetIntervalSec()
		{
			return clk::GetIntervalSec(start, stop);
		}
	};
}

// 
// Calculate FPS and print to stderr
//
static void PrintFPS(void)
{
	static __int64 t0 = -1;
	static int frames = 0;
	__int64 theTime;	
	__int64 t;
	float __fps = 0.0;

	theTime = clk::GetTime();
	t = clk::Sec(clk::GetTime());

	if (t0 < 0)
		t0 = t;

	frames++;

	if (t - t0 >= 5.0) 
	{
		float seconds = t - t0;
		__fps = frames / seconds;
		std::cout << frames << " frames in " << seconds << " seconds = " << __fps << " fps" <<std::endl;
		
		t0 = t;
		frames = 0;
	}
}

// 
// Calculate FPS and return as a float.
//
static float CalcFPS(void)
{
	static __int64 t0 = -1;
	static int frames = 0;
	__int64 theTime;	
	__int64 t;
	static float __fps = 0.0;

	theTime = clk::GetTime();
	t = theTime / 1000000000;

	if (t0 < 0)
		t0 = t;

	frames++;

	if (t - t0 >= 2.0) {
		float seconds = t - t0;
		__fps = frames / seconds;
		t0 = t;
		frames = 0;
	}

	return __fps;
}


#endif	//__CLOCK_TIME_H__