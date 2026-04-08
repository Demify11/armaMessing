#pragma once

#include <chrono>

class Timer {

private:
	const char* m_Tag;
	int m_SampleSize;

	std::chrono::steady_clock::time_point m_Start;

	long long m_TotalTime = 0;
	int m_Count = 0;

public:
	Timer(const char* tag, int sampleSize = 100)
		: m_Tag(tag), m_SampleSize(sampleSize) {}

	void Start();

	void Stop();
	

	
};