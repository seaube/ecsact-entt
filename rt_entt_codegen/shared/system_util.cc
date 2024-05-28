#include "system_util.hh"

#include <array>
#include <format>
#include <algorithm>
#include <map>
#include <unordered_map>

#include "ecsact/runtime/common.h"
#include "rt_entt_codegen/shared/util.hh"

auto ecsact::rt_entt_codegen::system_util::is_notify_system(
	ecsact_system_like_id system_id
) -> bool {
	auto count = ecsact::meta::system_notify_settings_count(system_id);

	if(count == 0) {
		return false;
	}

	auto notify_settings = ecsact::meta::system_notify_settings(system_id);

	for(const auto [comp_id, notify_setting] : notify_settings) {
		if(notify_setting == ECSACT_SYS_NOTIFY_ALWAYS ||
			 notify_setting == ECSACT_SYS_NOTIFY_NONE) {
			return false;
		}
	}

	return true;
}

auto ecsact::rt_entt_codegen::system_util::print_system_notify_views(
	ecsact::codegen_plugin_context&                            ctx,
	const ecsact::rt_entt_codegen::ecsact_entt_system_details& details,
	ecsact_system_like_id                                      system_id,
	std::string                                                registry_name
) -> void {
	using ecsact::cc_lang_support::c_identifier;
	using ecsact::cc_lang_support::cpp_identifier;
	using ecsact::cpp_codegen_plugin_util::block;
	using ecsact::meta::decl_full_name;

	auto notify_settings = ecsact::meta::system_notify_settings(system_id);

	using notify_settings_pair_t =
		std::pair<ecsact_component_like_id, ecsact_system_notify_setting>;

	auto notify_settings_vec = std::vector<notify_settings_pair_t>(
		notify_settings.begin(),
		notify_settings.end()
	);

	const auto notify_settings_ranking =
		std::map<ecsact_system_notify_setting, int>{
			{ECSACT_SYS_NOTIFY_ALWAYS, 99}, // unused
			{ECSACT_SYS_NOTIFY_NONE, 99}, // unused

			// We prioritize everything except onchange due to onchange having the
			// most overhead.
			{ECSACT_SYS_NOTIFY_ONINIT, 10},
			{ECSACT_SYS_NOTIFY_ONREMOVE, 9},
			{ECSACT_SYS_NOTIFY_ONUPDATE, 8},
			{ECSACT_SYS_NOTIFY_ONCHANGE, 0},
		};

	std::sort(
		notify_settings_vec.begin(),
		notify_settings_vec.end(),
		[&](notify_settings_pair_t a, notify_settings_pair_t b) -> bool {
			return notify_settings_ranking.at(a.second) >
				notify_settings_ranking.at(b.second);
		}
	);

	auto system_name = cpp_identifier(decl_full_name(system_id));

	for(auto const& [comp_id, notify_setting] : notify_settings_vec) {
		if(notify_setting == ECSACT_SYS_NOTIFY_ALWAYS ||
			 notify_setting == ECSACT_SYS_NOTIFY_NONE) {
			break;
		}
		auto cpp_comp_name = cpp_identifier(decl_full_name(comp_id));
		auto comp_name = c_identifier(decl_full_name((comp_id)));

		auto run_system_comp =
			std::format("ecsact::entt::detail::run_system<{}>", system_name);

		if(notify_setting == ECSACT_SYS_NOTIFY_ONINIT) {
			auto pending_add_str =
				std::format("ecsact::entt::component_added<{}>", cpp_comp_name);

			auto view_name = std::format("{}_init_view", comp_name);

			ecsact::rt_entt_codegen::util::make_view(
				ctx,
				view_name,
				registry_name,
				details,
				std::vector{pending_add_str},
				std::vector{run_system_comp}
			);

			block(ctx, std::format("for(auto entity: {})", view_name), [&]() {
				ctx.write(
					std::format("registry.emplace<{}>(entity);\n", run_system_comp)
				);
			});
		}

		if(notify_setting == ECSACT_SYS_NOTIFY_ONREMOVE) {
			auto pending_remove_str =
				std::format("ecsact::entt::component_removed<{}>", cpp_comp_name);

			auto view_name = std::format("{}_remove_view", comp_name);

			ecsact::rt_entt_codegen::util::make_view(
				ctx,
				view_name,
				registry_name,
				details,
				std::vector{pending_remove_str},
				std::vector{run_system_comp}
			);

			block(ctx, std::format("for(auto entity: {})", view_name), [&]() {
				ctx.write(
					std::format("registry.emplace<{}>(entity);\n", run_system_comp)
				);
			});
		}

		if(notify_setting == ECSACT_SYS_NOTIFY_ONUPDATE) {
			auto component_updated_str =
				std::format("ecsact::entt::component_updated<{}>", cpp_comp_name);

			auto view_name = std::format("{}_update_view", comp_name);

			ecsact::rt_entt_codegen::util::make_view(
				ctx,
				view_name,
				registry_name,
				details,
				std::vector{component_updated_str},
				std::vector{run_system_comp}
			);

			block(ctx, std::format("for(auto entity: {})", view_name), [&]() {
				ctx.write(
					std::format("registry.emplace<{}>(entity);\n", run_system_comp)
				);
			});
		}

		if(notify_setting == ECSACT_SYS_NOTIFY_ONCHANGE) {
			auto exec_itr_onchange_str = std::format(
				"ecsact::entt::detail::exec_itr_beforechange_storage<{}>",
				cpp_comp_name
			);

			auto view_name = std::format("{}_change_view", comp_name);

			ecsact::rt_entt_codegen::util::make_view(
				ctx,
				view_name,
				registry_name,
				details,
				std::vector{exec_itr_onchange_str},
				std::vector{run_system_comp}
			);

			block(ctx, std::format("for(auto entity: {})", view_name), [&]() {
				block(
					ctx,
					std::format(
						"if(!ecsact::entt::wrapper::core::has_component_changed<{}>(entity,"
						" registry))",
						cpp_comp_name
					),
					[&] { ctx.write("continue;\n"); }
				);

				ctx.write(
					std::format("registry.emplace<{}>(entity);\n", run_system_comp)
				);
			});
		}
	}
}

auto ecsact::rt_entt_codegen::system_util::is_trivial_system(
	ecsact_system_like_id system_id
) -> bool {
	using ecsact::meta::get_field_ids;
	using ecsact::meta::system_capabilities;

	auto sys_capabilities = system_capabilities(system_id);

	auto non_trivial_capabilities = std::array{
		ECSACT_SYS_CAP_READONLY,
		ECSACT_SYS_CAP_READWRITE,
		ECSACT_SYS_CAP_WRITEONLY,
		ECSACT_SYS_CAP_OPTIONAL_READONLY,
		ECSACT_SYS_CAP_OPTIONAL_READWRITE,
		ECSACT_SYS_CAP_OPTIONAL_WRITEONLY,
	};

	bool has_non_tag_adds = false;
	bool has_read_write = false;
	for(auto&& [comp_id, sys_cap] : sys_capabilities) {
		if((ECSACT_SYS_CAP_ADDS & sys_cap) == ECSACT_SYS_CAP_ADDS) {
			auto field_count =
				ecsact_meta_count_fields(ecsact_id_cast<ecsact_composite_id>(comp_id));
			if(field_count > 0) {
				has_non_tag_adds = true;
			}
		}

		for(auto non_trivial_cap : non_trivial_capabilities) {
			if((non_trivial_cap & sys_cap) == sys_cap) {
				has_read_write = true;
			}
		}
	}
	if(has_non_tag_adds || has_read_write) {
		return false;
	}
	return true;
}

auto ecsact::rt_entt_codegen::system_util::get_unique_view_name()
	-> std::string {
	static int counter = 0;
	return "view" + std::to_string(counter++);
}
