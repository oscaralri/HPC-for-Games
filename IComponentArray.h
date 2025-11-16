#pragma once
#include "ECSConfig.h"

namespace ECS {

	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void 

	private:
		// packed array of components
		std::array<T, MAX_ENTITIES> mComponentArray;
		
		// entity id to array index
		std::unordered_map<Entity, size_t> mEntityToIndexMap;

		// array index to entity id
		std::unordered_map<size_t, Entity> mIndexToEntityMap;

		size_t mSize;
	};

}
