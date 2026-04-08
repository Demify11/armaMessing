#include "Timer.h"

void Timer::Start() {
	m_Start = std::chrono::steady_clock::now();
}

void Timer::Stop() {

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_Start).count();

	m_TotalTime += duration;

	m_Count++;

	if (m_Count >= m_SampleSize) {
		double avg = (m_TotalTime / m_Count) / 1000.0;
		printf("[PROFILER] %s Avg: %.3f ms (%d samples)\n", m_Tag, avg, m_Count);

		m_TotalTime = 0;

		m_Count = 0;
	}

	/*
	auto Endpoint = std::chrono::high_resolution_clock::now();

	auto Start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
	auto End = std::chrono::time_point_cast<std::chrono::microseconds>(Endpoint).time_since_epoch().count();
	
	auto Duration = End - Start;
	const double TimeMs = Duration * 0.001f;

	printf("[BENCHMARK] %s Duration: %1f \n", m_Tag.c_str(), TimeMs);

	m_Stopped = true;
	*/
}

/*
Timer::~Timer() {
	if (!m_Stopped)
		Stop();
}
*/