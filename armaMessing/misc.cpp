#include "framework.h"

Vector3 Screen;
bool WorldToScreen(UINT64 Camera, Vector3 World, Vector3& Screen) {

    auto ViewRight = Coms->ReadVirtual<Vector3>(Camera + 0x8);
    auto ViewUp = Coms->ReadVirtual<Vector3>(Camera + 0x14);
    auto ViewAside = Coms->ReadVirtual<Vector3>(Camera + 0x20);
    auto ViewPosition = Coms->ReadVirtual<Vector3>(Camera + 0x2C);
    auto ViewPort = Coms->ReadVirtual<Vector3>(Camera + 0x58);
    auto ViewProjD1 = Coms->ReadVirtual<Vector3>(Camera + 0xD0);
    auto ViewProjD2 = Coms->ReadVirtual<Vector3>(Camera + 0xDC);

    auto Relative = Vector3{
        World.x - ViewPosition.x,
        World.y - ViewPosition.y,
        World.z - ViewPosition.z,
    };

    float _x = Relative.Dot(ViewRight);
    float _y = Relative.Dot(ViewUp);
    float _z = Relative.Dot(ViewAside);

    Screen = Vector3{
        ViewPort.x * (1 + (_x / ViewProjD1.x / _z)),
        ViewPort.y * (1 - (_y / ViewProjD2.y / _z)),
        _z
    };

    return _z >= 1.5f;
}



Vector3 CalculateAngles(Vector3 Local, Vector3 Target, Vector3 GunAngles) {

    // I didnt hear, you're a bit quiet . fair fair

    Vector3 Abseloute = {
        Target.x - Local.x,
        Target.y - Local.y,
        Target.z - Local.z,
    };
    // ok ok.

   // yes sir.
   // It's a direction vector.

   // So what we need is the..
   //i cant even remember lol

   // oh it's actually rather simple now that i think about it.
   // We just have to make a direction vector towards them.

   // so to do that, we need to normalize our the abseloute vector

   // it's taking a vector, and turning it's size into 1, while still keeping it's direction.

   // this?
    float XzDist = sqrtf(Abseloute.x * Abseloute.x + Abseloute.z * Abseloute.z);

    // yea, if you convert this to radians using atan. you get the view angles. Which is what we've actually done here
    Vector3 Angles = {
        Abseloute.x / XzDist,
        atan(Abseloute.y / XzDist),
        Abseloute.z / XzDist
    };

    // so the gunangles, are an offset of your current aim.
    // so let's say you're aiming 60 degrees on yaw.
    // then the gun is offsetted by 2 degrees.
    // Here we need the atan stuff you were dealing 
    // with. to combine these two values.

        // fucking english keyboard get that subnliigme shit out nig <-- written by demetri pls dont kill me eu

    // This is our current view angle.
    float ViewAngle = atan2f(Angles.x, Angles.z);   

    // Now we get were the gun is offset to
    float GunAngle = atan2f(GunAngles.x, GunAngles.z);

    // It's not doing the same thing :(
    // nah its fine,
    // i struggled a lot with this math
    // I recommend you using GeoGebra.org/Classic
    // this way you can visualize what's happening.
    // actually that program helped me solve this lol.
    // oh wait, hold on. It's due to character inverse rot.

    // Here we convert it back into a direction vector
    float RealAngle = ViewAngle + GunAngle;

    // yeah, switching values always best try lol
    // we havent' taken into consideration the y rn BIT OF PROOFCORE
    Angles = {
        sin(RealAngle), // 2D VEC-Direction X,
        GunAngles.y + Angles.y, // Radians
        cos(RealAngle)  // 2D VEC-Direction Z
    };

    // you wanna do the sway / recoil comp stuff now?
    // yea but the other is cooler 
    // yea yea.

    // we need some more stuff.
    // we just need gun angles. wrote it up here.

    // anyways pitch is very easy, however you need to get the adjacent side of a triangle constructed alongside the xz-plane and y axis.
    // and that sounds complicated, but it's rather easy. It's legit just atan(Abseloute.y / XzDist) sorry. well not really.
    // we're still using the square root values on this one, to normalize it. however we're getting some other information
    // 4 rad is near 360 degree's i ebelieve
    // yeah, you gucci then.
    // if you do no sway, no recoil, it's perfect lol

    // imma do paint real quick.
    // so a vector, is like a direction and a size.
    // this vector here has the values (2,2)
    // it's size is gonna be sqrt(x*x + z*z) which is wtf.
    // sqrt(8) = 2.8 smth like that.
    // HOWEVER let's make a vector with the values (1,1)

    // it has the same direction as the vector (2,2)
    // however it's size is not the same.
    // when you're working with arma, the values are between 0, 1
    // by "normalizing the vector" you keep the vectors direction intact, and scaling it down to be the size of 1.

    // by dividing the x,y values with the size of the vector, you keep the direction intact, 
    //   but you lower the vector's size to 1

    // i hope i explained it well enough, math is not my strongest lol.
    // math is cool, if you know it i guess.
    // i sucked at math.


    // yessir.
    // we can test what happens if we put in a value that's not normalized.

    return Angles; // I deleted it; oh; lol. Okay you need a couple of things; you need to decide how you want to implement your smoohting; 
}
/*
* I wanted to do some thing called critically damped spring smoothing/aimassist
* but fuck that
* ill just do this // thats gay as fuck. let me see what this is ^^
t Seems pretty cool; but I don't think you need to do all that to get smoothing; you just want this curve essentially; and way to increase or decrease the duration (D) and intensity (I) 
// please discord call this is aids.
//also that video from before was critically damped spring, 
u
*/


UINT64 GetLocalPlayer(const UINT64& World) {
    const auto LocalPlayerLink = Coms->ReadVirtual<UINT64>(World + 0x2C20);

    if (!LocalPlayerLink)
        return 0;

    return Coms->ReadVirtual<UINT64>(LocalPlayerLink + 0x8);
}

Vector3 GetEntityPosition(const UINT64& Entity) {
    const auto VisualState = Coms->ReadVirtual<UINT64>(Entity + 0xD0);

    return Coms->ReadVirtual<Vector3>(VisualState + 0x2C);
}

void Teleport(const UINT64& VisualState)
{

    Vector3 Forward = Coms->ReadVirtual<Vector3>(VisualState + 0x20);
    Vector3 Position = Coms->ReadVirtual<Vector3>(VisualState + 0x2c);

    constexpr float Distance = 20.f;

    Position.x += Distance * Forward.x;
    Position.z += Distance * Forward.z;

    Coms->WriteVirtual<Vector3>(VisualState + 0x2c, Position);
}

void noSway(const UINT64& ModuleBase)
{

    Coms->WriteVirtual<float>(ModuleBase + 0x25D20C4, 0);
    Coms->WriteVirtual<float>(ModuleBase + 0x25D20EC, 0);
}

bool b = true;
void noRecoil(const UINT64& ModuleBase)
{

    if (b) {
        /* auto recoil = Coms->Read<float>(ModuleBase + 0x1FEE84C);
         std::cout << recoil << std::endl;
         Coms->WriteVirtual<float>(ModuleBase + 0x1FEE84C, 0);
         b = !b;*/

        auto buff = Coms->ReadVirtual<UINT64>(ModuleBase + 0x2596C50);
        auto buff1 = Coms->ReadVirtual<UINT64>(buff + 0x2B00);
        auto buff2 = Coms->ReadVirtual<UINT64>(buff1 + 0x8);
        Coms->WriteVirtual<float>(buff2 + 0x1088, 0);
    }
    else {
        Coms->WriteVirtual<float>(ModuleBase + 0x26BFCE4, 1);
        b = !b;
    }


}

// Config needed
// - Weight
// - TimeToTarget
static Vector3 s_vel = { 0, 0, 0 };

float WrapPi(float a) {
    a = fmodf(a + 3.14159265f, 6.28318530f);
    if (a < 0.f) a += 6.28318530f;
    return a - 3.14159265f;
}

float YawFromHybrid(const Vector3& h) {
    return atan2f(h.x, h.z);
}

Vector3 HybridFromYawPitch(float yaw, float pitch) {
    return Vector3(sinf(yaw), pitch, cosf(yaw));
}
float SmoothDamp(float current, float target, float& vel,
    float smoothTime, float dt) {
    // smoothTime = roughly how long, in seconds, to reach the target
    float omega = 2.f / smoothTime;

    float x = omega * dt;
    // Padé approximation of exp(-x) — cheaper than calling expf every frame
    float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    float temp = (vel + omega * change) * dt;

    vel = (vel - omega * temp) * exp;
    return target + (change + temp) * exp;
}

float SmoothDampAngle(float cur, float target, float& vel,
    float smoothTime, float dt) {
    target = cur + WrapPi(target - cur);   // <-- the only addition
    return SmoothDamp(cur, target, vel, smoothTime, dt);
}

AimSmoother g_AimSmoother;
Vector3 AimSmoother::Update(Vector3 cur, Vector3 tgt, float dt, float smoothTime) {
    if (smoothTime < 1e-4f) smoothTime = 1e-4f;  // avoid omega = inf -> snap
    if (dt <= 0.f) return cur;                    // nothing to integrate this frame

    float curYaw = atan2f(cur.x, cur.z);          // live view yaw
    float tgtYaw = atan2f(tgt.x, tgt.z);          // target yaw

    float yaw = SmoothDampAngle(curYaw, tgtYaw, yawVel, smoothTime, dt);
    float pitch = SmoothDamp(cur.y, tgt.y, pitchVel, smoothTime, dt);

    const float HalfPi = 1.57079633f;
    pitch = fmaxf(-HalfPi, fminf(HalfPi, pitch)); // real clamp

    return Vector3(sinf(yaw), pitch, cosf(yaw));  // back to (dirX, pitch, dirZ)
}


Vector3 SmoothingUnderdamped(Vector3 CurrentAngles, Vector3 TargetAngles, float Time, float TimeToTarget) {
    constexpr float Weight = 2.5f;
    float TempPercentage = Time / TimeToTarget;
    float Percentage = 1 - expf(-Weight * TempPercentage) * (cosf(Weight * TempPercentage) + sin(Weight * TempPercentage));
    auto Result = CurrentAngles + (TargetAngles - CurrentAngles) * Percentage;
    // clamp your stuff; icbf
    if (Result.y > 3.1415 / 2) {
        __debugbreak();
    }
    else if (Result.y < -(3.1415 / 2)) {
        __debugbreak();
    }
    return Result;
}

Entity* bestTarget(std::vector<Entity*> entities, std::vector<Vehicle*> Vehicles, UINT64 ModuleBase)
{
    Entity* bestT = nullptr;
    float closest = FLT_MAX;

    const auto Camera = g_Client->GetWorld()->GetCamera();

    for (auto& entity : entities) {
        
        Vector3 Pos;

        if (entity->GetHeadPosition().IsZero())
            continue;

        if (entity->alive) {
            if (g_Client->GetWorld()->GetCamera()->WorldToScreen(entity->GetHeadPosition(), Pos)) {

                if (!g_Client->GetWorld()->IsInFOV(Pos))
                    continue;
                    
                Vector3 viewPort = Camera->CachedViewPort;

                Vector3 centre;
                centre.x = viewPort.x;
                centre.y = viewPort.y;

                float distance = sqrtf(
                    powf(Pos.x - centre.x, 2) + powf(Pos.y - centre.y, 2)
                );

                if (distance < closest) {
                    closest = distance;
                    bestT = entity; //should we return pointer or entity
                }

                //float distance = sqrtf(powf((Pos.x - centre.x), 2) + powf((Pos.y - centre.y), 2));
                //if (distance < closest) {
                //    closest = distance;
                //    bestT = CurrentEnt.GetHeadPosition();
                //}
            }
        }
    }

    for (auto& CurrentEnt : Vehicles)
    {
        //const auto World = Coms->ReadVirtual<UINT64>(ModuleBase + 0x2596C50);
        Vector3 Pos;
        //const auto Camera = Coms->ReadVirtual<UINT64>(World + 0xD30);

        //     if (!CurrentEnt.GetDead()) {
        //CurrentEnt->GetTargetInVehicleTransform();
        if (g_Client->GetWorld()->GetCamera()->WorldToScreen(CurrentEnt->m_TransformedHeadPos, Pos)) {

            Vector3 viewPort = Camera->CachedViewPort;
            Vector3 centre;
            centre.x = viewPort.x;
            centre.y = viewPort.y;

            float distance = sqrtf(powf((Pos.x - centre.x), 2) + powf((Pos.y - centre.y), 2));
            if (distance < closest) {
                closest = distance;
                bestT = CurrentEnt;
            }
            //}
        }
    }

    return bestT;
}

#include "World.h"

void HeadESP(const std::vector<Entity*> entities , UINT64 World, std::vector<Vehicle*> vehicles) {
    
    auto Pre = Coms->GetReads();

    Camera* Camera = g_Client->GetWorld()->GetCamera();
    
    for (const auto& entity : entities) {
        Vector3 ScreenPos;
        //if (WorldToScreen(pCamera, entity.GetHeadPosition(), ScreenPos)) {//maby use getheadpostion2
        if (Camera->WorldToScreen(entity->GetHeadPosition(), ScreenPos)) {

            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenPos.x, ScreenPos.y), 5, ImColor(0, 0, 200));
        }
    }
    for (const auto& entity : vehicles) {

        auto Draw = ImGui::GetBackgroundDrawList();

        Vector3 ScreenPos;
        //entity->GetTargetInVehicleTransform();
        if (Camera->WorldToScreen(entity->m_TransformedHeadPos, ScreenPos)) {

            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenPos.x, ScreenPos.y), 5, ImColor(200, 0, 0));
        }

    }
    
    auto Post = Coms->GetReads();

    //printf("[DEBUG] \t-READS on HEADESP: %i \n", Post - Pre);

}

void ESP(const std::vector<Entity*>& entityMap,const std::vector<Vehicle*>& vehicleMap, uint64_t worldBase, uint64_t moduleBase) {

    auto Draw = ImGui::GetBackgroundDrawList();
    
    const auto Camera = g_Client->GetWorld()->GetCamera();

    for (const auto& entity : entityMap) {

        //if (entity->type != EntityType::Player || entity->type != EntityType::Vehicle)
        //    break;

        Vector3 FeetPosition = entity->GetFeetPosition();
        Vector3 HeadPosition = entity->GetHeadPosition();     // this is messed up, <-- this aint no head position, this is camera position. You are looping entities in here. but only getting the head position at the local player. so in reality, wrong information.


        Vector3 ScreenFeet;
        Vector3 ScreenHead;

        if (FeetPosition.x == 0.0f && FeetPosition.z == 0.0f)
            continue;
        /*
        float Distance = sqrtf(
            powf(FeetPosition.x - LocalPosition.x, 2) +
            powf(FeetPosition.y - LocalPosition.y, 2) +
            powf(FeetPosition.z - LocalPosition.z, 2)
        );

        if (Distance > 2000)
            continue;*/


        if (Camera->WorldToScreen(FeetPosition, ScreenFeet) &&
            Camera->WorldToScreen(HeadPosition, ScreenHead)) {

            float Height = ScreenHead.y - ScreenFeet.y;
            float Width = Height / 2;

            auto x = ScreenFeet.x - Width / 2;
            auto y = ScreenFeet.y;

            x = floorf(x); //imgui gets sus when rendering partial pixel.
            y = floorf(y);
            //Width = floorf(Width);
            //Height = floorf(Height);

            ImVec2 rect_min = ImVec2(x, y); 
            ImVec2 rect_max = ImVec2(x + Width, y + Height);
            if (entity) { //a blank entity sometimes is in entity list 0xd2d2d2d2 and causes crash with imgui func. this is getto fix
                auto Name = entity->m_Name.c_str();
                auto Size = ImGui::CalcTextSize(Name);
                auto FinalPositionX = ScreenHead.x - Size.x / 2;

                Draw->AddText(ImVec2(FinalPositionX, ScreenHead.y), IM_COL32_WHITE, entity->m_Name.c_str());
            }
            /*
            Draw->AddRect(ImVec2(x, y), ImVec2(x + Width, y + Height), ImColor(255, 255, 255), 0, 0, 1);
            Draw->AddRect(ImVec2(rect_min.x + 1, rect_min.y + 1), ImVec2(rect_max.x - 1, rect_max.y - 1), ImColor(0, 0, 0));
            Draw->AddRect(ImVec2(rect_min.x - 1, rect_min.y - 1), ImVec2(rect_max.x + 1, rect_max.y + 1), ImColor(0, 0, 0));
            */

            Draw->AddRect(rect_min, rect_max, ImColor(0, 0, 0), 0.0f, 0, 3.0f);
            // white inner line
            Draw->AddRect(rect_min, rect_max, ImColor(255, 255, 255), 0.0f, 0, 1.5f);
            

        }

    }

    for (const auto ent : vehicleMap) {

        Vector3 ScreenPos;

        if (Camera->WorldToScreen(ent->GetFeetPosition(), ScreenPos)) {

            Vector3 FeetWorld = ent->GetFeetPosition();
            Vector3 HeadWorld = FeetWorld + Vector3(0, 0, 40.0f);

            Vector3 FeetScreen, HeadScreen;
            if (!Camera->WorldToScreen(FeetWorld, FeetScreen)) return;
            
            float depth = FeetScreen.z;
            float Size = 1000.0f / depth;
            Size = std::clamp(Size, 5.0f, 40.0f);

            float Width = Size;
            float Height = Size;

            float CenterX = FeetScreen.x;
            float CenterY = FeetScreen.y - Height * 0.5f; // lift box up off the feet, ImGui Y-down
            
            ImVec2 TopL(CenterX - Width * 0.5f, CenterY - Height * 0.5f);
            ImVec2 BotR(CenterX + Width * 0.5f, CenterY + Height * 0.5f);

            Draw->AddRect(TopL, BotR, ImColor(0, 0, 0), 0.0f, 0, 3.0f);
            // white inner line
            Draw->AddRect(TopL, BotR, ImColor(220, 77, 1), 0.0f, 0, 1.5f);

        }
    }
}