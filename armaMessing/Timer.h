#pragma once

#include <chrono>

class Timer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartPoint;
	std::string m_Tag = "";
	bool m_Stopped = false;

public:
	Timer();
	Timer(std::string Tag);
	~Timer();

public:
	void Stop();
};