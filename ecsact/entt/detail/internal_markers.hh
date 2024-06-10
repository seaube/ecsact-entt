#pragma once

#include <cstdint>
#include <type_traits>
#include "entt/entt.hpp"
#include "ecsact/entt/entity.hh"
#include "ecsact/runtime/common.h"

namespace ecsact::entt::detail {

template<typename C, std::size_t FieldOffset>
struct association {
	using component = C;
	static constexpr auto field_offset = FieldOffset;
	std::int_fast16_t     ref_count = 0;
};

template<typename Assoc>
concept association_concept = //
	requires {
		{ typename Assoc::component_type{} };
		{ Assoc::field_offset } -> std::convertible_to<std::size_t>;
	};

template<typename C>
struct beforeremove_storage;

template<typename C>
	requires(std::is_empty_v<C>)
struct beforeremove_storage<C> {};

template<typename C>
	requires(!std::is_empty_v<C>)
struct beforeremove_storage<C> {
	C value;
};

template<typename C>
	requires(!std::is_empty_v<C>)
struct exec_beforechange_storage {
	C    value;
	bool has_update_occurred = false;
};

template<typename C>
	requires(!std::is_empty_v<C>)
struct exec_itr_beforechange_storage {
	C value;
};

template<typename C>
struct pending_add;

template<typename C>
	requires(std::is_empty_v<C>)
struct pending_add<C> {};

template<typename C>
	requires(!std::is_empty_v<C>)
struct pending_add<C> {
	C value;
};

template<typename C>
struct pending_remove {};

struct created_entity {
	ecsact_placeholder_entity_id placeholder_entity_id;
};

struct destroyed_entity {};

template<typename S>
struct system_sorted {
	std::uint64_t hash = 0;
	friend auto operator<=>(const system_sorted&, const system_sorted&) = default;
};

template<typename S>
struct pending_lazy_execution {};

template<typename S>
struct run_system {};

template<typename>
constexpr bool system_markers_unimplemented_by_codegen = false;

template<typename C>
auto add_system_markers_if_needed( //
	ecsact::entt::registry_t&,
	ecsact::entt::entity_id
) -> void {
	static_assert(system_markers_unimplemented_by_codegen<C>, R"(
 -----------------------------------------------------------------------------
| (!) CODEGEN ERROR                                                           |
| `add_system_markers_if_needed<>` template specialization cannot be found.   |
| This is typically generated by ecsact_rt_entt_codegen.                      |
 -----------------------------------------------------------------------------
)");
}

template<typename C>
auto remove_system_markers_if_needed( //
	ecsact::entt::registry_t&,
	ecsact::entt::entity_id
) -> void {
	static_assert(system_markers_unimplemented_by_codegen<C>, R"(
 -----------------------------------------------------------------------------
| (!) CODEGEN ERROR                                                           |
| `remove_system_markers_if_needed<>` template specialization cannot be found |
| This is typically generated by ecsact_rt_entt_codegen.                      |
 -----------------------------------------------------------------------------
)");
}

template<typename C>
auto add_exec_itr_beforechange_if_needed( //
	ecsact::entt::registry_t&,
	ecsact::entt::entity_id
) -> void {
	static_assert(system_markers_unimplemented_by_codegen<C>, R"(
 -----------------------------------------------------------------------------
| (!) CODEGEN ERROR                                                           |
| `add_exec_itr_beforechange_if_needed<>` template specialization cannot be found |
| This is typically generated by ecsact_rt_entt_codegen.                      |
 -----------------------------------------------------------------------------
)");
}

} // namespace ecsact::entt::detail
