
#pragma once
#include"Framework.h"
#include "SessionHandoff.h"
#include "AppSession.h"
#undef GetProcessId // have to undefine the default GetProcessId cause haram interference >:(


Overlay* g_Overlay = new Overlay;
MemInterface* Coms = new MemInterface;
Client* g_Client   = new Client;
SigScanner* g_SigScanner = new SigScanner;

static NetworkManager g_net{ "http://localhost:3000" };
static std::unique_ptr<AppSession> g_session;
static std::atomic<bool> g_shutdown{ false };


#pragma section(".lic", read, write)
__declspec(allocate(".lic"))
LicenseContext g_LicenseInfo = { 0xC0FFEE01 };

#include "Timer.h"

void CacheThread() {
    
    auto LastSlowCache = std::chrono::steady_clock::now();
    int i = 0;

    int Tick = 16;
    auto NextTick = std::chrono::steady_clock::now();

    int TicksPerSecond = 0;

    while (TRUE) {

        NextTick += std::chrono::milliseconds(Tick);
        auto Now = std::chrono::steady_clock::now();
        if (Now < NextTick) {
            std::this_thread::sleep_until(NextTick);
        }

        auto StartTime = std::chrono::steady_clock::now(); // Track loop start time

        static Timer CacheTimer("CACHE", 100);
        CacheTimer.Start();

        bool doSlowCache = false;
        auto Now_2 = std::chrono::steady_clock::now();

        while (Now_2 - LastSlowCache >= std::chrono::milliseconds(500)) {

            printf("[DEBUG] AVG. UPS %i \n", TicksPerSecond * 2);

            LastSlowCache += std::chrono::milliseconds(500);
            doSlowCache = true;

            TicksPerSecond = 0;
        }

        TicksPerSecond++;

        /*
        i++;
            if (i == 50) {
               // g_Client->m_World.pushclearpls();
                i = 0;
            }
            */
     

        g_Client->Cache(doSlowCache);

        CacheTimer.Stop();

#if _DEBUG
        //printf("[CACHE] %i - Reads on Tick: %i \n", doSlowCache, Coms->PopReads());
#endif

        auto EndTime = std::chrono::steady_clock::now();
        auto ElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime).count();

        // If we're lagging behind, resync to avoid spiral of death
        if (ElapsedMs > Tick) {
            NextTick = std::chrono::steady_clock::now();
        }
    }
}

void AttachLogConsole() {

    AllocConsole();
    
    FILE* f;

    freopen_s(&f, "CONOUT$", "w", stdout);

    //freopen_s("CONOUT$", "w", stdout);

}

DWORD WINAPI MainThread(LPVOID lpParam) {
    Sleep(1000);

    LicenseContext ctx;
    memcpy(&ctx, &g_LicenseInfo, sizeof(ctx));


    g_session = std::make_unique<AppSession>(g_net, std::string(ctx.refreshToken), std::string(ctx.licenseId), std::string(ctx.processId), ctx.interval);
    
    SecureZeroMemory(&g_LicenseInfo, sizeof(g_LicenseInfo));

    g_session->start();

    DWORD Pid;

    AttachLogConsole();

    if (!Coms->Init()) {
        printf("nigga");
    }

    extern UINT64 ModuleBase;

    ModuleBase = Coms->GetProcessBase(L"arma3_x64.exe", &Pid);

    
    //if (!ModuleBase)
        //MessageBoxA(NULL, "Test", "Test", MB_OK);

    auto EProcess = Coms->GetEProcess(Pid);
    
    //if (!EProcess)
        //MessageBoxA(NULL, "Test2", "Test", MB_OK);

    auto ProcessCR3 = Coms->GetProcessCR3(EProcess);

    //if (!ProcessCR3)
        //MessageBoxA(NULL, "Test3", "Test", MB_OK);

    Coms->SetTarget(ProcessCR3);

    g_Client->ModuleBase = ModuleBase;

    if (Coms->ReadVirtual<WORD>(ModuleBase) != IMAGE_DOS_SIGNATURE) { printf("something wrong\n"); }
    //printf("[INFO] ModuleBase 0x%llX \n", ModuleBase);

    g_SigScanner->Init();

    extern UINT64 WorldAddr;
    WorldAddr = Coms->ReadVirtual<UINT64>(ModuleBase + Offsets::World);

    const auto CameraOnRef = Coms->ReadVirtual<UINT64>(WorldAddr + Offsets::World);

    auto CameraOn = Entity();
    CameraOn.m_Base = Coms->ReadVirtual<UINT64>(CameraOnRef + 0x8);


    if (!g_Overlay->Init()) {
        // printf("Failed to load overlay \n");
        exit(1);
    }

    CloseHandle(
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CacheThread, 0, 0, 0)
    );

    g_Overlay->Loop();
}

//int i = 1;
#ifdef _DEBUG



int main() {

    LicenseContext ctx;
    memcpy(&ctx, &g_LicenseInfo, sizeof(ctx));
    SecureZeroMemory(&g_LicenseInfo, sizeof(g_LicenseInfo));
    if (ctx.magic != 0xC0FFEE01) {
        printf("Failed to Handover info");
    }

    DWORD Pid;
    Coms->Init();
    extern UINT64 ModuleBase;

    ModuleBase = Coms->GetProcessBase(L"arma3_x64.exe",&Pid);

    auto EProcess = Coms->GetEProcess(Pid);

    auto ProcessCR3 = Coms->GetProcessCR3(EProcess);

    Coms->SetTarget(ProcessCR3);

    g_Client->ModuleBase = ModuleBase;


    if (Coms->ReadVirtual<WORD>(ModuleBase) != IMAGE_DOS_SIGNATURE) { printf("something wrong\n"); }
    //printf("[INFO] ModuleBase 0x%llX \n", ModuleBase);

    g_SigScanner->Init();

    extern UINT64 WorldAddr;
    WorldAddr = Coms->ReadVirtual<UINT64>(ModuleBase + Offsets::World);


    //printf("[INFO] World 0x%llX \n", World);

    const auto CameraOnRef = Coms->ReadVirtual<UINT64>(WorldAddr + Offsets::World);

    auto CameraOn = Entity();
    CameraOn.m_Base = Coms->ReadVirtual<UINT64>(CameraOnRef + 0x8);

    
    if (!g_Overlay->Init()) {
       // printf("Failed to load overlay \n");
        exit(1);
    }
    
    CloseHandle(
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CacheThread, 0, 0, 0)
    );
    
    g_Overlay->Loop();
    
    
}
#else

BOOL APIENTRY DllMain(HMODULE hModule,DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        //DisableThreadLibraryCalls(hModule);

        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }

    return TRUE;
}



#endif