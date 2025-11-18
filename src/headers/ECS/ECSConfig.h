#pragma once

#include <cstdint>
#include <bitset>
#include <cassert>

namespace ECS {
	// ENTITIES
	using Entity = std::uint32_t;
	constexpr Entity MAX_ENTITIES = 5000;

	// COMPONENTS
	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32; 

	// SIGNATURE
	using Signature = std::bitset<MAX_COMPONENTS>;
}
