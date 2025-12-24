
/*struct Vector2 {
    float x, t;
};*/
void PushbackEntity(std::vector<Vehicle>& Vehicles,std::vector<Entity>& List, UINT64 World, UINT32 Offset, UINT64 LocalPlayer);
bool WorldToScreen(UINT64 Camera, Vector3 World, Vector3& Screen);
Vector3 CalculateAngles(Vector3 Local, Vector3 Target, Vector3 GunAngles);
UINT64 GetLocalPlayer(const UINT64& World);
Vector3 GetEntityPosition(const UINT64& Entity);
void Teleport(const UINT64& VisualState);
void noSway(const UINT64& ModuleBase);
void noRecoil(const UINT64& ModuleBase);
Vector3 bestTarget(std::vector<Entity> Entities, std::vector<Vehicle> Vehicles, UINT64 ModuleBase);
void HeadESP(std::vector<Entity> Entities, UINT64 World, std::vector<Vehicle> Vehicles);
void ESP(const std::unordered_map<uintptr_t, Entity>& entityMap, uint64_t worldBase, uint64_t moduleBase);
void GatherEntitiesAndVehiclesAtOffset(uint64_t worldBase, uint32_t offset, uint64_t localPlayer, std::unordered_map<uintptr_t, Entity>& entities, std::unordered_map<uintptr_t, Vehicle>& vehicles);

template <typename T>
void UpdateMapWithDelta(
    std::unordered_map<uintptr_t, T>& currentMap,
    const std::unordered_map<uintptr_t, T>& incomingMap
) {
    for (const auto& [base, incomingEntity] : incomingMap) {
        auto it = currentMap.find(base);
        if (it == currentMap.end()) {
            // New entity
            currentMap[base] = incomingEntity;
        }
        else {
            // Existing entity: update if needed (up to you to define "changed")
            // In practice, you might compare important fields here.
            it->second = incomingEntity;
        }
    }

    // Optionally, remove stale entities:
    for (auto it = currentMap.begin(); it != currentMap.end(); ) {
        if (incomingMap.find(it->first) == incomingMap.end()) {
            it = currentMap.erase(it);
        }
        else {
            ++it;
        }
    }
}