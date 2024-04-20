#include "rt_entt_codegen/core/core.hh"

#include <set>
#include <format>
#include "ecsact/runtime/meta.hh"
#include "ecsact/lang-support/lang-cc.hh"
#include "ecsact/cpp_codegen_plugin_util.hh"
#include "rt_entt_codegen/shared/sorting.hh"

using ecsact::cpp_codegen_plugin_util::block;
using ecsact::cpp_codegen_plugin_util::method_printer;
using ecsact::rt_entt_codegen::system_needs_sorted_entities;

auto ecsact::rt_entt_codegen::core::print_system_marker_add_fn(
	codegen_plugin_context&    ctx,
	const ecsact_entt_details& details
) -> void {
	for(auto comp : details.all_components) {
		auto comp_like_id = ecsact_id_cast<ecsact_component_like_id>(comp);
		auto comp_name = ecsact::meta::decl_full_name(comp);
		auto comp_cpp_ident = cc_lang_support::cpp_identifier(comp_name);
		auto sorting_structs_covered = std::set<ecsact_system_id>{};

		ctx.write("template<>\n");
		auto printer =
			method_printer{
				ctx,
				"ecsact::entt::detail::add_system_markers_if_needed<" + comp_cpp_ident +
					">"
			}
				.parameter("::entt::registry&", "reg")
				.parameter("ecsact::entt::entity_id", "entity")
				.return_type("void");

		for(auto system_id : details.all_systems) {
			if(sorting_structs_covered.contains(system_id)) {
				continue;
			}

			auto sys_details = ecsact_entt_system_details::from_system_like(
				ecsact_id_cast<ecsact_system_like_id>(system_id)
			);

			if(!sys_details.readable_comps.contains(comp_like_id)) {
				continue;
			}

			if(system_needs_sorted_entities(system_id, sys_details)) {
				auto system_name = ecsact::meta::decl_full_name(system_id);
				auto system_cpp_ident =
					"::" + cc_lang_support::cpp_identifier(system_name);

				auto system_sorting_struct_name =
					std::format("system_sorted<{}>", system_cpp_ident);

				block(
					ctx,
					"if(::ecsact::entt::entity_matches_system<" + system_cpp_ident +
						">(reg, entity))",
					[&] {
						ctx.write(
							"reg.emplace_or_replace<",
							system_sorting_struct_name,
							">(entity);"
						);
					}
				);
			}
		}
	}
}

auto ecsact::rt_entt_codegen::core::print_system_marker_remove_fn(
	codegen_plugin_context&    ctx,
	const ecsact_entt_details& details
) -> void {
	for(auto comp : details.all_components) {
		auto comp_like_id = ecsact_id_cast<ecsact_component_like_id>(comp);
		auto comp_name = ecsact::meta::decl_full_name(comp);
		auto comp_cpp_ident = cc_lang_support::cpp_identifier(comp_name);
		auto sorting_structs_covered = std::set<ecsact_system_id>{};

		ctx.write("template<>\n");
		auto printer =
			method_printer{
				ctx,
				"ecsact::entt::detail::remove_system_markers_if_needed<" +
					comp_cpp_ident + ">"
			}
				.parameter("::entt::registry&", "reg")
				.parameter("ecsact::entt::entity_id", "entity")
				.return_type("void");

		ctx.write("//TODO\n");
	}
}