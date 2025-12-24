#pragma once
/* Windows & Crt Libraries */
#include <Windows.h>
#include <dwmapi.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <TlHelp32.h>
#include <intrin.h>
#include <intrin.h>
#include <Psapi.h>
#include <winternl.h>
#include "mem.h"
#include "AutoArray.h"

/* DirectX & ImGui */
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

/* GAME CLASSES */
#include "Offsets.h"
#include "Signatures.h"
#include "Magazine.h"
#include "Weapon.h"
#include "Entity.h"
#include "World.h"
#include "misc.h"
#include "SigScanner.h"

/* Overlay & Rendering */
#include "Overlay.h"


extern void Teleport(const UINT64& VisualState);