#include "Timer.h"

Timer::Timer() {
	m_StartPoint = std::chrono::high_resolution_clock::now();
}

Timer::Timer(std::string Tag) {
	m_StartPoint = std::chrono::high_resolution_clock::now();
	m_Tag = Tag;
}

Timer::~Timer() {
	if (!m_Stopped)
		Stop();
}

void Timer::Stop() {

	auto Endpoint = std::chrono::high_resolution_clock::now();

	auto Start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
	auto End = std::chrono::time_point_cast<std::chrono::microseconds>(Endpoint).time_since_epoch().count();
	
	auto Duration = End - Start;
	const double TimeMs = Duration * 0.001f;

	printf("[BENCHMARK] %s Duration: %1f \n", m_Tag.c_str(), TimeMs);

	m_Stopped = true;
}