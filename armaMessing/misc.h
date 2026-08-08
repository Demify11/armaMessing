
/*struct Vector2 {
    float x, t;
};*/
bool WorldToScreen(UINT64 Camera, Vector3 World, Vector3& Screen);
Vector3 CalculateAngles(Vector3 Local, Vector3 Target, Vector3 GunAngles);
UINT64 GetLocalPlayer(const UINT64& World);
Vector3 GetEntityPosition(const UINT64& Entity);
void Teleport(const UINT64& VisualState);
void noSway(const UINT64& ModuleBase);
void noRecoil(const UINT64& ModuleBase);
Entity* bestTarget(std::vector<Entity*> entities, std::vector<Vehicle*> Vehicles, UINT64 ModuleBase);
Vector3 SmoothingUnderdamped(Vector3 CurrentAngles, Vector3 TargetAngles, float Time, float TimeToTarget);
void HeadESP(const std::vector<Entity*> entities, UINT64 World, std::vector<Vehicle*> vehicles);
void ESP(const std::vector<Entity*>& entityMap,const std::vector<Vehicle*>& vehicleMap, uint64_t worldBase, uint64_t moduleBase);

float WrapPi(float a);
float YawFromHybrid(const Vector3& h);
Vector3 HybridFromYawPitch(float yaw, float pitch);
float SmoothDamp(float current, float target, float& vel,float smoothTime, float dt);
float SmoothDampAngle(float cur, float target, float& vel,float smoothTime, float dt);
struct AimSmoother {
    float yawVel = 0.f, pitchVel = 0.f;
    Vector3 lastOut;
    bool init = false;
    Entity* PrevTarget = nullptr;

    Vector3 prevAimPoint;
    bool havePrev = false;

    // Call once when a smooth begins, seeding from the live camera so
    // it starts where you're actually looking.
    void Seed(const Vector3& currentHybrid) {
        lastOut = currentHybrid;
        yawVel = pitchVel = 0.f;
        init = true;
    }

    Vector3 Update(Vector3 currentHybrid, Vector3 targetHybrid, float dt, float smoothTime);
};

extern AimSmoother g_AimSmoother;