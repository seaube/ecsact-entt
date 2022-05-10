#include <ecsact/runtime/dynamic.h>

#include <boost/mp11.hpp>

#include "common.template.hh"
#include "system_execution_context.hh"

using namespace ecsact_entt_rt;

namespace {
	using package = typename decltype(ecsact_entt_rt::runtime)::package;
}

template<typename Fn>
static void cast_and_use_ctx
	( ecsact_system_execution_context*  ctx
	, Fn&&                              fn
	)
{
	using boost::mp11::mp_for_each;
	using boost::mp11::mp_identity;
	using boost::mp11::mp_transform;
	using boost::mp11::mp_flatten;
	using boost::mp11::mp_push_back;

	using all_systems = mp_flatten<mp_push_back<
		typename package::actions,
		typename package::systems
	>>;
	using all_systems_identities = mp_transform<mp_identity, all_systems>;

	mp_for_each<all_systems_identities>([&]<typename S>(mp_identity<S>) {
		// S is a system or an action so we must cast the potential action id to a
		// system id.
		if(static_cast<::ecsact::system_id>(S::id) == ctx->system_id) {
			fn(*static_cast<system_execution_context<package, S>*>(ctx->impl));
		}
	});
}

const void* ecsact_system_execution_context_action
	( ecsact_system_execution_context*  context
	)
{
	return context->impl->action;
}

void ecsact_system_execution_context_add
	( ecsact_system_execution_context*  context
	, ecsact_component_id               component_id
	, const void*                       component_data
	)
{
	cast_and_use_ctx(context, [&](auto& context) {
		context.add(
			static_cast<::ecsact::component_id>(component_id),
			component_data
		);
	});
}

void ecsact_system_execution_context_remove
	( ecsact_system_execution_context*  context
	, ecsact_component_id               component_id
	)
{
	cast_and_use_ctx(context, [&](auto& context) {
		context.remove(static_cast<::ecsact::component_id>(component_id));
	});
}

void ecsact_system_execution_context_get
	( ecsact_system_execution_context*  context
	, ecsact_component_id               component_id
	, void*                             out_component_data
	)
{
	using boost::mp11::mp_for_each;

	mp_for_each<typename package::components>([&]<typename C>(C) {
		if constexpr(!std::is_empty_v<C>) {
			if(C::id == static_cast<::ecsact::component_id>(component_id)) {
				cast_and_use_ctx(context, [&](auto& context) {
					C& out_component = *reinterpret_cast<C*>(out_component_data);
					out_component = context.template get<C>();
				});
			}
		}
	});
}

void ecsact_system_execution_context_update
	( ecsact_system_execution_context*  context
	, ecsact_component_id               component_id
	, const void*                       component_data
	)
{
	using boost::mp11::mp_for_each;

	mp_for_each<typename package::system_writables>([&]<typename C>(C) {
		if(C::id == static_cast<::ecsact::component_id>(component_id)) {
			cast_and_use_ctx(context, [&](auto& context) {
				const C& comp = *reinterpret_cast<const C*>(component_data);
				context.template update<C>(comp);
			});
		}
	});
}

bool ecsact_system_execution_context_has
	( ecsact_system_execution_context*  context
	, ecsact_component_id               component_id
	)
{
	bool has_component = false;

	cast_and_use_ctx(context, [&](auto& context) {
		has_component = context.has(
			static_cast<::ecsact::component_id>(component_id)
		);
	});

	return has_component;
}

const ecsact_system_execution_context* ecsact_system_execution_context_parent
	( ecsact_system_execution_context*  context
	)
{
	return context->impl->parent;
}

bool ecsact_system_execution_context_same
	( const ecsact_system_execution_context* a
	, const ecsact_system_execution_context* b
	)
{
	return a->impl->entity == b->impl->entity;
}

void ecsact_system_execution_context_generate
	( ecsact_system_execution_context*  context
	, int                               component_count
	, ecsact_component_id*              component_ids
	, const void**                      components_data
	)
{
	cast_and_use_ctx(context, [&](auto& context) {
		context.generate(
			component_count,
			reinterpret_cast<::ecsact::component_id*>(component_ids),
			components_data
		);
	});
}

#ifdef ECSACT_ENTT_RUNTIME_DYNAMIC_SYSTEM_IMPLS
bool ecsact_set_system_execution_impl
	( ecsact_system_id              system_id
	, ecsact_system_execution_impl  system_exec_impl
	)
{
	return runtime.set_system_execution_impl(
		static_cast<::ecsact::system_id>(system_id),
		system_exec_impl
	);
}
#endif

ecsact_system_id ecsact_system_execution_context_id
	( ecsact_system_execution_context* context
	)
{
	return static_cast<ecsact_system_id>(context->system_id);
}
