#pragma once

#include <set>
#include "ECSConfig.h"

namespace ECS {
	class System
	{
	public:
		std::set<Entity> mEntites;
	};
}


