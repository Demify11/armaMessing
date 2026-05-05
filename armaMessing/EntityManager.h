#pragma once
#include <unordered_set>


class EntityManager {
public:
    void Tick(bool State);

    const std::vector<Entity*> GetEntities() { 
        entityMutex.lock();
        auto& buffer = entities; 
        entityMutex.unlock();
        return buffer;
    }
    
    const std::vector<Entity*> GetVehicles() { 
        entityMutex.lock();
        auto& buffer = vehicles;
        entityMutex.unlock();
        return buffer;
    }

private:
    std::unordered_map<uintptr_t, Entity> entityMap;
    std::unordered_set<uintptr_t> knownBases;

    std::mutex entityMutex;
    std::vector<Entity*> entities;
    std::vector<Entity*> vehicles;

    bool m_Changed = true;

    void DetectAndUpdate();
    void RebuildVectors();
};