#include "framework.h"

void EntityManager::Tick(bool State) {

    if(State)
        DetectAndUpdate();

    /*
    for (auto& [_, entity] : entityMap) {
        entity.Cache(State); we are using the vectors and not the map to cache anymore because vector itterations are faster, and
        i changed it so that the vectors hold pointers, so now we can just cache them directly instead of caching the map.
    }*/

    for (auto* entity : entities) {
        entity->Cache(State);
    }

    for (auto* entity : vehicles) {
        entity->Cache(State);
    }
}

void EntityManager::DetectAndUpdate() {

    std::unordered_set<uintptr_t> currentBases;
    m_Changed = false;

    //  CHEAP
    for (uint32_t offset : { Offsets::EntityListNearNear, Offsets::EntityListNear, Offsets::EntityListFar}) {

        auto Array = AutoArray(g_Client->m_World.m_Base + offset);

        if (!Array.Allocate(Array.GetSize()))
            return;

        for (int i = 0; i < Array.GetSize(); i++) {

            auto Base = Array.Get(i);

            if (!Base || Base == g_Client->m_World.m_LocalPlayer.m_Base)
                continue;

            currentBases.insert(Base);

            //Handle New entities
            if (!knownBases.contains(Base)) {
                if (Entity* e = Entity::Create(Base)) {

                    entityMap.emplace(Base, std::move(e)); //this is faster
                    m_Changed = true;
                }
            }
        }
    }

    //Remove Dead Entities
    for (auto it = entityMap.begin(); it != entityMap.end(); ) {
        if (!currentBases.contains(it->first)) {
            it = entityMap.erase(it);
            m_Changed = true;
        }
        else {
            ++it;
        }
    }

    //Save Latest State
    knownBases = std::move(currentBases);

    //We Rebuild
    if (m_Changed)
        RebuildVectors();
}

void EntityManager::RebuildVectors() {

    std::vector<Entity*> tempEntities;
    std::vector<Vehicle*> tempVehicles;

    tempEntities.reserve(entityMap.size());
    tempVehicles.reserve(entityMap.size());

    for (auto& [_, entity] : entityMap) {
        
        if (entity->type == EntityType::Vehicle) {
            tempVehicles.push_back(static_cast<Vehicle*>(entity));
        } else {
            tempEntities.push_back(entity);
        }
    }

    entityMutex.lock(); 
    entities = std::move(tempEntities);
    vehicles = std::move(tempVehicles);
    entityMutex.unlock();
}