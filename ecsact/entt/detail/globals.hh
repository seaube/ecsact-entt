#pragma once

#include <unordered_map>
#include <unordered_set>
#include "entt/entity/registry.hpp"
#include "ecsact/runtime/common.h"
#include "ecsact/runtime/core.h"
#include "ecsact/runtime/dynamic.h"
#include "ecsact/entt/detail/system_execution_context.hh"
#include "ecsact/entt/detail/registry.hh"

/**
 * A small set of globals expected to be available the ecsact_rt_entt_codegen
 * generated source.
 */
namespace ecsact::entt::detail::globals {

/**
 * Ecsact registry ID mapped to EnTT registry instance.
 */
extern std::unordered_map< //
	ecsact_registry_id,
	ecsact::entt::registry_t>
	registries;

/**
 * Stored last registry ID. Used to create new registry IDs.
 */
extern ecsact_registry_id last_registry_id;

/**
 * System-like implementation functions set by dynamic API.
 */
extern std::unordered_map< //
	ecsact_system_like_id,
	ecsact_system_execution_impl>
	system_impls;

/**
 * All components IDs available to rt_entt
 */
extern const std::unordered_set<ecsact_component_id> all_component_ids;

/**
 * ecsact_add_component fn pointers
 *
 * NOTE: This gets is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_component_id,
	decltype(&ecsact_add_component)>
	add_component_fns;

/**
 * ecsact_get_component fn pointers
 *
 * NOTE: This gets is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_component_id,
	decltype(&ecsact_get_component)>
	get_component_fns;

/**
 * ecsact_update_component fn pointers
 *
 * NOTE: This gets is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_component_id,
	decltype(&ecsact_update_component)>
	update_component_fns;

/**
 * ecsact_remove_component fn pointers
 *
 * NOTE: This gets is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_component_id,
	decltype(&ecsact_remove_component)>
	remove_component_fns;

/**
 * ecsact_has_component fn pointers
 *
 * NOTE: This gets is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_component_id,
	decltype(&ecsact_has_component)>
	has_component_fns;

/**
 * ecsact_system_execution_context_get fn pointers
 *
 * NOTE: This gets is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_system_like_id,
	std::unordered_map<
		ecsact::entt::assoc_index,
		std::unordered_map< //
			ecsact_component_like_id,
			decltype(&ecsact_system_execution_context_get)>>>
	exec_ctx_get_fns;

/**
 * ecsact_system_execution_context_add fn pointers
 *
 * NOTE: This adds is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_system_like_id,
	std::unordered_map<
		ecsact::entt::assoc_index,
		std::unordered_map< //
			ecsact_component_like_id,
			decltype(&ecsact_system_execution_context_add)>>>
	exec_ctx_add_fns;

/**
 * ecsact_system_execution_context_update fn pointers
 *
 * NOTE: This updates is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_system_like_id,
	std::unordered_map<
		ecsact::entt::assoc_index,
		std::unordered_map< //
			ecsact_component_like_id,
			decltype(&ecsact_system_execution_context_update)>>>
	exec_ctx_update_fns;

/**
 * ecsact_system_execution_context_remove fn pointers
 *
 * NOTE: This removes is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_system_like_id,
	std::unordered_map<
		ecsact::entt::assoc_index,
		std::unordered_map< //
			ecsact_component_like_id,
			decltype(&ecsact_system_execution_context_remove)>>>
	exec_ctx_remove_fns;

/**
 * ecsact_system_execution_context_has fn pointers
 *
 * NOTE: This has is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map< //
	ecsact_system_like_id,
	std::unordered_map<
		ecsact::entt::assoc_index,
		std::unordered_map< //
			ecsact_component_like_id,
			decltype(&ecsact_system_execution_context_has)>>>
	exec_ctx_has_fns;

/**
 * ecsact_system_execution_context_action fn pointers
 *
 * NOTE: This action is filled in by ecsact_rt_entt_codegen
 */
extern const std::unordered_map<
	ecsact_action_id,
	decltype(&ecsact_system_execution_context_action)>
	exec_ctx_action_fns;

} // namespace ecsact::entt::detail::globals
