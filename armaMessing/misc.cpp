#include "framework.h"


void PushbackEntity(std::vector<Vehicle>& Vehicles,std::vector<Entity>& List, UINT64 World, UINT32 Offset, UINT64 LocalPlayer) {

    const auto ListEntry = Coms->ReadVirtual<UINT64>(World + Offset);

    const auto ListSize = Coms->ReadVirtual<UINT32>(World + Offset + 0x8);

    for (auto i = 0; i < ListSize; i++) {

        const auto EntityEntry = Coms->ReadVirtual<UINT64>(ListEntry + (i * 0x8));
        const auto CatagoryBuff = Coms->ReadVirtual<UINT64>(EntityEntry + 0x150);
        const auto Catagory = Coms->ReadVirtual<UINT64>(CatagoryBuff + 0xD0);
        std::string CatagoryString = Coms->ReadString(Catagory + 0x10);

        if (CatagoryString == "carx") {
            Vehicle Temp;
            Temp.m_Base = EntityEntry;
            //Vehicles.push_back(Temp);
            continue;
        }
        if (CatagoryString == "invisible") {
            continue;
        }
        if (EntityEntry == LocalPlayer)
            continue;

        Entity Temp;
        Temp.m_Base = EntityEntry;

        List.push_back(Temp);
    }
}

void GatherEntitiesAndVehiclesAtOffset(uint64_t worldBase,uint32_t offset,uint64_t localPlayer,std::unordered_map<uintptr_t, Entity>& entities,std::unordered_map<uintptr_t, Vehicle>& vehicles)
{
    const auto listEntry = Coms->ReadVirtual<uint64_t>(worldBase + offset);
    if (!listEntry) return;

    const auto listSize = Coms->ReadVirtual<uint32_t>(worldBase + offset + 0x8);
    if (listSize == 0 || listSize > 1024) return;

    for (uint32_t i = 0; i < listSize; ++i) {
        const auto entityBase = Coms->ReadVirtual<uint64_t>(listEntry + i * 0x8);
        if (!entityBase || entityBase == localPlayer)
            continue;

        const auto catBuf = Coms->ReadVirtual<uint64_t>(entityBase + 0x150);
        if (!catBuf) continue;

        const auto catPtr = Coms->ReadVirtual<uint64_t>(catBuf + 0xD0);
        if (!catPtr) continue;

        const std::string category = Coms->ReadString(catPtr + 0x10);
        if (category == "invisible") continue;

        const bool alive = (Coms->ReadVirtual<UINT8>(entityBase + 0x5CC) & 1);
        if (!alive == 0) continue;

        if (category == "carx") {
            Vehicle v;
            v.m_Base = entityBase;
            vehicles[entityBase] = v;
            continue;
        }

        Entity e;
        e.m_Base = entityBase;
        entities[entityBase] = e;
    }
}

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
    // Here we need the atan stuff you were dealing with. to combine these two values.

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
        sin(RealAngle),
        GunAngles.y + Angles.y,
        cos(RealAngle)
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

    return Angles;

}

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

Vector3 bestTarget(std::vector<Entity> Entities, std::vector<Vehicle> Vehicles, UINT64 ModuleBase)
{
    Vector3 bestT;
    float closest = FLT_MAX;

    const auto Camera = g_Client->GetWorld()->GetCamera();

    for (auto& CurrentEnt : Entities) {

        CurrentEnt.Cache(false);    // dummy stuff.

        const auto World = Coms->ReadVirtual<UINT64>(ModuleBase + 0x2596C50);
        Vector3 Pos;

        //const auto Camera = Coms->Read<UINT64>(World + 0xD30);

        if (CurrentEnt.GetHeadPosition().IsZero())
            continue;

        if (CurrentEnt.dead) {
            if (g_Client->GetWorld()->GetCamera()->WorldToScreen(CurrentEnt.GetHeadPosition(), Pos)) {

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
                    bestT = CurrentEnt.GetHeadPosition();
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
        const auto World = Coms->ReadVirtual<UINT64>(ModuleBase + 0x2596C50);
        Vector3 Pos;
        const auto Camera = Coms->ReadVirtual<UINT64>(World + 0xD30);

        //     if (!CurrentEnt.GetDead()) {
        CurrentEnt.GetTargetInVehicle(CurrentEnt.HHeadPos);
        if (g_Client->GetWorld()->GetCamera()->WorldToScreen(CurrentEnt.HHeadPos, Pos)) {

            Vector3 viewPort = Coms->ReadVirtual<Vector3>(Camera + 0x58);
            Vector3 centre;
            centre.x = viewPort.x;
            centre.y = viewPort.y;

            float distance = sqrtf(powf((Pos.x - centre.x), 2) + powf((Pos.y - centre.y), 2));
            if (distance < closest) {
                closest = distance;
                bestT = CurrentEnt.HHeadPos;
            }
            //}
        }
    }

    return bestT;
}

void HeadESP(std::vector<Entity> Entities, UINT64 World, std::vector<Vehicle> Vehicles) {
    const auto pCamera = Coms->ReadVirtual<UINT64>(World + 0xD30);
    for (Entity& entity : Entities) {
        Vector3 ScreenPos;
        if (WorldToScreen(pCamera, entity.GetHeadPosition(), ScreenPos)) {//maby use getheadpostion2

            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenPos.x, ScreenPos.y), 5, ImColor(0, 0, 200));
        }
    }

    Camera* Camera = g_Client->GetWorld()->GetCamera();

    for (Vehicle& vehicle : Vehicles) {

        Vector3 Position;
        Vector3 Aside;
        Vector3 Up;
        Vector3 Front;

        vehicle.GetVehicleTransform(Aside, Up, Front, Position);

        Aside = Position + (Aside * 3);
        Up = Position + (Up * 3);
        Front = Position + (Front * 3);

        Vector3 AsideScreen;
        Vector3 UpScreen;
        Vector3 FrontScreen;
        Vector3 PositionScreen;

        auto Draw = ImGui::GetBackgroundDrawList();

        if (Camera->WorldToScreen(Aside, AsideScreen) &&
            Camera->WorldToScreen(Up, UpScreen) &&
            Camera->WorldToScreen(Front, FrontScreen) &&
            Camera->WorldToScreen(Position, PositionScreen)) {

            Draw->AddLine(ImVec2(PositionScreen.x, PositionScreen.y), ImVec2(AsideScreen.x, AsideScreen.y), ImColor(255,255,255));
            Draw->AddLine(ImVec2(PositionScreen.x, PositionScreen.y), ImVec2(UpScreen.x, UpScreen.y), ImColor(255, 255, 255));
            Draw->AddLine(ImVec2(PositionScreen.x, PositionScreen.y), ImVec2(FrontScreen.x, FrontScreen.y), ImColor(255,255,255));
        }

        Vector3 ScreenPos;
        vehicle.GetTargetInVehicle(vehicle.HHeadPos);
        if (WorldToScreen(pCamera, vehicle.HHeadPos, ScreenPos)) {

            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenPos.x, ScreenPos.y), 5, ImColor(200, 0, 0));
        }
    }
}

void ESP(const std::unordered_map<uintptr_t, Entity>& entityMap, uint64_t worldBase, uint64_t moduleBase) {
    auto Draw = ImGui::GetBackgroundDrawList();
    
    const auto Camera = g_Client->GetWorld()->GetCamera();

    for (const auto& [base, entity] : entityMap) {

        // nothing in entity is cached yet?
        // -> burger joint.

        //if (Entity.GetDead())
        //    continue;

        Vector3 FeetPosition = entity.GetFeetPosition();
        Vector3 HeadPosition = entity.GetHeadPosition();     // this is messed up, <-- this aint no head position, this is camera position. You are looping entities in here. but only getting the head position at the local player. so in reality, wrong information.


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

        // when translating stuff from world into view perspective, be careful of what values you are dealing with.
        // if you want to draw something from the world onto the monitor, you have to go trhough W2S and use the SCREEN cords to draw from.
        // this code was just wrong cause i did it wrong, but yea.
        // Now. You cache properly after this.

        // voila, w2s without so many reads

        if (Camera->WorldToScreen(FeetPosition, ScreenFeet) &&
            Camera->WorldToScreen(HeadPosition, ScreenHead)) {

            float Height = ScreenHead.y - ScreenFeet.y;
            float Width = Height / 2;

            auto x = ScreenFeet.x - Width / 2;
            auto y = ScreenFeet.y;

            Draw->AddRect(ImVec2(x, y), ImVec2(x + Width, y + Height), ImColor(255, 255, 255), 0, 0, 1);

        }

        //if (WorldToScreen(Camera, FeetPosition, ScreenFeet) &&
        //    WorldToScreen(Camera, HeadPosition, ScreenHead)) {
        //
        //    float Height = ScreenHead.y - ScreenFeet.y;
        //    float Width = Height / 2;
        //
        //    auto x = ScreenFeet.x - Width / 2;
        //    auto y = ScreenFeet.y;
        //
        //    Draw->AddRect(ImVec2(x, y), ImVec2(x + Width, y + Height), ImColor(255, 255, 255), 0, 0, 10);
        //
        //}

    }
}