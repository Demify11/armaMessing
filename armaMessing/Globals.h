#pragma once
#include "framework.h"

extern std::unique_ptr<AppSession> g_session;
extern std::atomic<bool> g_shutdown;

extern bool g_Enabled;
extern float g_Fov;
extern bool bEsp;
extern bool bHESP;
extern bool bShowNames;
extern bool bShowDistance;
extern bool bShowFov;
extern bool bNoRecoil;
extern bool bNoSway;
extern bool bAimBot;