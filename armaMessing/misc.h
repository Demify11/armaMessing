
/*struct Vector2 {
    float x, t;
};*/
void PushbackEntity(std::vector<Entity>& Vehicles,std::vector<Entity>& List, UINT64 World, UINT32 Offset, UINT64 LocalPlayer);
bool WorldToScreen(UINT64 Camera, Vector3 World, Vector3& Screen);
Vector3 CalculateAngles(Vector3 Local, Vector3 Target, Vector3 GunAngles);
UINT64 GetLocalPlayer(const UINT64& World);
Vector3 GetEntityPosition(const UINT64& Entity);
void Teleport(const UINT64& VisualState);
void noSway(const UINT64& ModuleBase);
void noRecoil(const UINT64& ModuleBase);
Entity bestTarget(std::vector<Entity*> entities, std::vector<Vehicle*> Vehicles, UINT64 ModuleBase);
void HeadESP(const std::vector<Entity*> entities, UINT64 World, std::vector<Vehicle*> vehicles);
void ESP(const std::vector<Entity*>& entityMap, uint64_t worldBase, uint64_t moduleBase);
void GatherEntitiesAndVehiclesAtOffset(uint64_t worldBase, uint32_t offset, uint64_t localPlayer, std::unordered_map<uintptr_t, Entity>& entities, std::unordered_map<uintptr_t, Entity>& vehicles);
