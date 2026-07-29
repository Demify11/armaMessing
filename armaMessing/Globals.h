#pragma once
#include "framework.h"

extern std::unique_ptr<AppSession> g_session;
extern std::atomic<bool> g_shutdown;