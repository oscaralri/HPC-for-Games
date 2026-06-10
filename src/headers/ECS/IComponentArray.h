#pragma once
#include "ECSConfig.h"
#include <unordered_map>
#include <vector>
#include <cassert>

namespace ECS {

    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(Entity entity) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray {
    public:
        ComponentArray() {
            mComponentVector.reserve(MAX_ENTITIES);
        }

        void InsertData(Entity entity, T component) {
            assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

            size_t newIndex = mComponentVector.size();
            mEntityToIndexMap[entity] = newIndex;
            mIndexToEntityMap[newIndex] = entity;

            mComponentVector.push_back(component);
        }

        void RemoveData(Entity entity) {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "RemoveData: Removing non-existent component.");

            size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
            size_t indexOfLastElement = mComponentVector.size() - 1;

            mComponentVector[indexOfRemovedEntity] = mComponentVector[indexOfLastElement];

            Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
            mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            mEntityToIndexMap.erase(entity);
            mIndexToEntityMap.erase(indexOfLastElement);

            mComponentVector.pop_back();
        }

        T& GetData(Entity entity) {
            auto it = mEntityToIndexMap.find(entity);
            assert(it != mEntityToIndexMap.end() && "GetData: Retrieving non-existent component.");
            return mComponentVector[it->second];
        }

        void EntityDestroyed(Entity entity) override {
            if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
                RemoveData(entity);
            }
        }

    private:
        std::vector<T> mComponentVector;

        std::unordered_map<Entity, size_t> mEntityToIndexMap;
        std::unordered_map<size_t, Entity> mIndexToEntityMap;
    };
}