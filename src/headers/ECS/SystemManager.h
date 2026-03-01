#pragma once

#include <memory>
#include <unordered_map>

#include "ECSConfig.h"
#include "System.h"


namespace ECS {
	class SystemManager
	{
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();

			assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

			auto system = std::make_shared<T>();
			mSystems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();

			assert(mSystems.find(typeName) != mSystems.end() && "System used before registered");

			mSignatures.insert({ typeName, signature });
		}
		
		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : mSystems)
			{
				auto const& system = pair.second;
				RemoveEntityFromSystem(system->mEntities, entity);
			}
		}

		void EntitySignatureChanged(Entity entity, Signature entitySignature)
		{
			for (auto const& pair : mSystems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = mSignatures[type];

				bool matches = (entitySignature & systemSignature) == systemSignature;

				auto it = std::find(system->mEntities.begin(), system->mEntities.end(), entity);
				bool exists = (it != system->mEntities.end());

				if (matches && !exists)
				{
					system->mEntities.push_back(entity);
				}
				else if (!matches && exists)
				{
					RemoveEntityFromSystem(system->mEntities, entity);
				}
			}
		}

		template<typename T>
		std::shared_ptr<T> GetSystem()
		{
			const char* typeName = typeid(T).name();

			auto it = mSystems.find(typeName);
			assert(it != mSystems.end() && "System not found!");

			return std::static_pointer_cast<T>(it->second);
		}

	private:
		std::unordered_map<const char*, Signature> mSignatures{};
		std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};

		void RemoveEntityFromSystem(std::vector<Entity>& entities, Entity entity)
		{
			auto it = std::find(entities.begin(), entities.end(), entity);
			if (it != entities.end())
			{
				*it = entities.back();
				entities.pop_back();
			}
		}
	};

}

