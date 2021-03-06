#pragma once
#include <chrono>

// This header contains a class that maintains the time keeping for the engine and,
// maintains the time elapsed between frames for physics calculation.

// It is a singleton class.
class Time
{
	Time() {}

public:
	static Time* GetInstance()
	{
		static Time* instance = new Time();
		return instance;
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> startTime;
	float stepTime = 0.0f;
	float prevStepTime = 0.0f;

public:
	// Function to give the time/duration since the start function is called by the engine.(in millisecond)
	double GetCurrentStartTime();
	// Function to start the time of game.
	void Start();
	// Function to call when the frame starts.
	void ResetFrameTime();
	// Function to get the time it took for previous frame to execute.(in millisecond)
	float GetDeltaTime();
};
