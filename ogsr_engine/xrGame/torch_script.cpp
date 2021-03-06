#include "stdafx.h"
#include "torch.h"
#include "script_game_object.h"
#include "inventory_item_object.h"
#include "../xr_3da/LightAnimLibrary.h"

IRender_Light *CTorch::GetLight(int target) const
{
	if (target == 0)
		return light_render._get();
	else
		if (light_omni)
			return light_omni._get();

	return nullptr;
}


void CTorch::SetAnimation(LPCSTR name)
{
	lanim = LALib.FindItem(name);
}

void CTorch::SetBrightness(float brightness)
{
	auto c = light_render->get_color();
	if (fBrightness > 0)
		c.mul_rgb(255.f / fBrightness);

	fBrightness = brightness;
	SetColor(c); // immediate update
}

void CTorch::SetColor(const Fcolor &color, int target)
{
	auto c = color;
	c.mul_rgb(fBrightness / 255.f);
	switch (target)
	{
	case 0: light_render->set_color(c); break;
	case 1: if (light_omni) light_omni->set_color(c); break;
	case 2: if (glow_render) glow_render->set_color(c); break;
	}

}

void CTorch::SetRGB(float r, float g, float b, int target)
{
	Fcolor c;
	c.a = 1;
	c.r = r;
	c.g = g;
	c.b = b;
	SetColor(c, target);
}

void CTorch::SetAngle(float angle, int target)
{
	switch (target)
	{
	case 0: light_render->set_cone(angle); break;
	case 1: if (light_omni) light_omni->set_cone(angle); break;
	}
}

void CTorch::SetRange(float range, int target)
{
	switch (target)
	{
	case 0: light_render->set_range(range); break;
	case 1: if (light_omni) light_omni->set_range(range); break;
	case 2: if (glow_render) glow_render->set_radius(range); break;
	}
}

void CTorch::SetTexture(LPCSTR texture, int target)
{
	switch (target)
	{
	case 0: light_render->set_texture(texture); break;
	case 1: if (light_omni) light_omni->set_texture(texture); break;
	case 2: if (glow_render) glow_render->set_texture(texture); break;
	}
}
void CTorch::SetVirtualSize(float size, int target)
{
	switch (target)
	{
	case 0: light_render->set_virtual_size(size);
	case 1: if (light_omni) light_omni->set_virtual_size(size);
	}
}



CTorch *get_torch(CScriptGameObject *script_obj)
{
	auto obj = &script_obj->object();
	auto t = smart_cast<CTorch*> (obj);
	if (t) return t;
	script_obj = script_obj->GetObjectByName("device_torch");
	if (script_obj) return
		get_torch(script_obj); // ��������
	return nullptr;
}


using namespace luabind;
#pragma optimize("s",on)
void CTorch::script_register(lua_State *L)
{
	module(L)
		[
		class_<CTorch, CGameObject /*CInventoryItemObject*/ >("CTorch")
		.def(constructor<>())
		// alpet: ���������� ����������� �����
		.def_readonly("on", &CTorch::m_switched_on)
		.def("enable", (void (CTorch::*)(bool)) (&CTorch::Switch))
		.def("switch", (void (CTorch::*)())	 (&CTorch::Switch))
		.def("get_light", &CTorch::GetLight)
		.def("set_animation", &CTorch::SetAnimation)
		.def("set_angle", &CTorch::SetAngle)
		.def("set_brightness", &CTorch::SetBrightness)
		.def("set_color", &CTorch::SetColor)
		.def("set_rgb", &CTorch::SetRGB)
		.def("set_range", &CTorch::SetRange)
		.def("set_texture", &CTorch::SetTexture)
		.def("set_virtual_size", &CTorch::SetVirtualSize)
		// ������ � ���
		.def_readonly("nvd_on", &CTorch::m_bNightVisionOn)
		.def("enable_nvd", (void (CTorch::*)(bool)) (&CTorch::SwitchNightVision))
		.def("switch_nvd", (void (CTorch::*)()) (&CTorch::SwitchNightVision))

		,
		def("get_torch_obj", &get_torch)
		];
}
