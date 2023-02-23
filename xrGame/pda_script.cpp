#include "pch_script.h"
#include "pda.h"

using namespace luabind;

#pragma optimize("s",on)
void CPda::script_register(lua_State* L)
{
	module(L)
		[
			class_<CPda, CGameObject>("CPda")
			.def(constructor<>())
		.def("get_hacked_status", &CPda::get_hacked_status)
		.def("set_hacked_status", &CPda::set_hacked_status)
		];
}
