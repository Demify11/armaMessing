// Arma3ConsoleRadar.cpp : This file contains the 'main' function. Program execution begins and ends there.
//[[[<arma3_x64.exe>+2674500]+2B00]+8]
//[[[[<arma3_x64.exe>+26733A0]+2B30]+8]+D0]
//#pragma once
#include"Framework.h"
#undef GetProcessId // have to undefine the default GetProcessId cause haram interference >:(


Overlay* g_Overlay = new Overlay;
MemInterface* Coms = new MemInterface;
Client* g_Client   = new Client;
SigScanner* g_SigScanner = new SigScanner;

#include "Timer.h"

void CacheThread() {
    
    auto LastSlowCache = std::chrono::steady_clock::now();
    int i = 0;

    while (TRUE) {
        auto StartTime = std::chrono::steady_clock::now(); // Track loop start time

        auto Benchmark = Timer("CACHE");

        auto Current = std::chrono::steady_clock::now();
        auto Delta = std::chrono::duration_cast<std::chrono::milliseconds>(Current - LastSlowCache).count();

        if (Delta >= 500) {
            LastSlowCache += std::chrono::milliseconds(500);  // Maintain fixed step
            i++;

            if (i == 50) {
               // g_Client->m_World.pushclearpls();
                i = 0;
            }
        }

        g_Client->Cache(Delta >= 500);

        Benchmark.Stop();

        printf("[CACHE] %i - Reads on Tick: %i \n", Delta >= 500, Coms->PopReads());
        
        auto EndTime = std::chrono::steady_clock::now();
        auto ElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime).count();

        int SleepDuration = 1 - static_cast<int>(ElapsedMs); // Ensure at least 1ms sleep

        if (SleepDuration > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SleepDuration));
        }
    }
}

//int i = 1;
int main() {

    DWORD Pid;
    Coms->Init();
    extern UINT64 ModuleBase;
    ModuleBase = Coms->GetProcessBase(L"arma3_x64.exe",&Pid);

    auto EProcess = Coms->GetEProcess(Pid);

    auto ProcessCR3 = Coms->GetProcessCR3(EProcess);

    Coms->SetTarget(ProcessCR3);

    g_Client->ModuleBase = ModuleBase;

    //printf("[INFO] ModuleBase 0x%llX \n", ModuleBase);

    g_SigScanner->Init();

    extern UINT64 WorldAddr;
    WorldAddr = Coms->ReadVirtual<UINT64>(ModuleBase + Offsets::ModBase::World);


    //printf("[INFO] World 0x%llX \n", World);

    const auto CameraOnRef = Coms->ReadVirtual<UINT64>(WorldAddr + 0x2C38);

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


    /*
    while (true) {


        // Clear Console
        system("cls");
        printf("[INFO] ModuleBase 0x%llX \n", ModuleBase);
        printf("is suicide the answer?");

        // Same as before, jsut differently setup.

        const auto CameraOnRef = Coms->Read(World + 0x2B00);

        auto CameraOn = Entity();
        CameraOn.Address = Coms->Read(CameraOnRef + 0x8);

        auto Entities = std::vector<Entity>();

        PushbackEntity(Entities, World, 0x1B08 + 8, CameraOn.Address);// We have to do + 8, cause at offset 0x0 is the class vtable.
        PushbackEntity(Entities, World, 0x1BD0 + 8, CameraOn.Address);
        PushbackEntity(Entities, World, 0x1C98 + 8, CameraOn.Address);
        PushbackEntity(Entities, World, 0x1D60 + 8, CameraOn.Address);

        printf("[INFO] Entity Size: %i \n", (unsigned int)Entities.size());
        std::cout << "hello" << std::endl;

    
        Entity TargetEntity;
        // You would normally have some form of target selection.
        // So you'd have a list of entities, and then sort.
        // First you check if they are invis
        // - then dead
        // - then far away (like 2000m +)
        // - then if they're in fov
        // if all of these criterias are hit, you get the one closest to the crosshair / center.
        // and then you do aimbot on that cunt.
        // you can calculate that yourself.
        // you already have everything needed
        // 
        //TargetEntity = Entities[2];
        //auto Angles = CalculateAngles(CameraOn.GetHeadPosition2(ModuleBase), TargetEntity.GetHeadPosition(), CameraOn.GetGunAngles());

        auto LocalPlayer1 = GetLocalPlayer(World);


        if (Entities.size() > 2 && i-1 != Entities.size()) {
            if (GetAsyncKeyState(VK_RBUTTON)) {
                TargetEntity = bestTarget(Entities,ModuleBase);

                    auto Angles = CalculateAngles(CameraOn.GetHeadPosition2(ModuleBase), TargetEntity.GetHeadPosition(), CameraOn.GetGunAngles());
                   //auto smoothPenisAngle = Angles / 2;
                   auto LocalPlayerBuff = Coms->Read<UINT64>(LocalPlayer1 + 0xD0);
                   auto ViewAngleBuff = Coms->Read<Vector3>(LocalPlayerBuff + 0x08);

                    //auto finalAngles = ViewAngleBuff - smoothPenisAngle;
                    //Sleep(100);

                    auto delta = Angles - ViewAngleBuff;
                    
                    CameraOn.WriteViewAngles(Angles);
                
                
            }

        }

        

        // 200 meter distance lol.
        // 20 y 20 grid
        // remember arma works with a xz-plane

        constexpr auto ConsoleWidth = 20i32;
        constexpr auto ConsoleHeight = 20i32;

        const auto LocalPlayer = GetLocalPlayer(World);
        const auto LocalPosition = GetEntityPosition(LocalPlayer);
        const auto VisualState = Coms->Read<UINT64>(LocalPlayer+0xD0);


        const auto GridSize = 200.0f / ConsoleWidth;

        const auto WorldTopX = LocalPosition.x - 200 / 2;
        const auto WorldTopZ = LocalPosition.z - 200 / 2;

        const auto WorldBotX = WorldTopX + 200;
        const auto WorldBotZ = WorldTopZ + 200;

        printf("WorldTop %.2f %.2f \n", WorldTopX, WorldTopZ);
        printf("WorldBot %.2f %.2f \n", WorldBotX, WorldBotZ);


        if (GetAsyncKeyState(VK_NUMPAD2)&1)
        {
            Teleport(VisualState);
        }

        if (GetAsyncKeyState(VK_NUMPAD5)&1)
        {
            noRecoil(ModuleBase);
        }
        
        if (GetAsyncKeyState(VK_NUMPAD8)&1)
        {
            noSway(LocalPlayer);
        }

        
        /*
        for (int y = 0; y < ConsoleWidth; y++) {

            for (int x = 0; x < ConsoleHeight; x++) {
                const auto GridPosX = WorldTopX + (x * GridSize);
                const auto GridPosZ = WorldTopX + (y * GridSize);


                // check if entity exists in this square.
                bool AlreadyPainted = false;

                for (auto& Entity : Entities) {

                    if (AlreadyPainted)
                        break;

                    const auto Position = GetEntityPosition(Entity);


                    if ((GridPosX < Position.x && Position.x < (GridPosX + GridSize)) &&
                        (GridPosZ < Position.z && Position.z < (GridPosZ + GridSize))) {

                        printf("x  ");
                        AlreadyPainted = true;
                    }
                }

                if (!AlreadyPainted)
                    printf("   ");

            }

            printf("\n");
        }

        Sleep(100);
    }*/
    
}
