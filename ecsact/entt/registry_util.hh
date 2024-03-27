#pragma once

#include <cassert>
#include "entt/entity/registry.hpp"
#include "ecsact/entt/detail/globals.hh"

namespace ecsact::entt {

inline auto get_registry( //
	ecsact_registry_id id
) -> ::entt::registry& {
	using ecsact::entt::detail::globals::registries;

	// Check to make sure we're not trying to get a registry that doesn't exist
	// or has been destroyed.
	assert(registries.contains(id));
	return registries.at(id);
}

inline auto create_registry()
	-> std::tuple<ecsact_registry_id, ::entt::registry&> {
	using ecsact::entt::detail::globals::last_registry_id;
	using ecsact::entt::detail::globals::registries;

	auto registry_id = static_cast<ecsact_registry_id>(
		++reinterpret_cast<int32_t&>(last_registry_id)
	);
	auto& registry = registries[registry_id];

	return {registry_id, std::ref(registry)};
}

} // namespace ecsact::entt
