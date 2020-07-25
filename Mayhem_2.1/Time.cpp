#include "Time.h"

double Time::GetCurrentStartTime()
{
	auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - this->startTime);
	return elapsed.count();
}

void Time::Start()
{
	this->startTime = std::chrono::high_resolution_clock::now();
	this->stepTime = this->GetCurrentStartTime();
}

void Time::ResetFrameTime()
{
	this->prevStepTime = this->stepTime;
	this->stepTime = this->GetCurrentStartTime();
}

float Time::GetDeltaTime()
{
	float deltaTime = this->stepTime - this->prevStepTime;
	return deltaTime;
}
