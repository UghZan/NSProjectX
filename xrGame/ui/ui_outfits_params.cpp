#include "stdafx.h"
#include "ui_outfits_params.h"
#include "UIStatic.h"
#include "../object_broker.h"
#include "UIXmlInit.h"

CUIOutfitParams::CUIOutfitParams()
{
	Memory.mem_fill(m_info_items, 0, sizeof(m_info_items));
}

CUIOutfitParams::~CUIOutfitParams()
{
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];
		xr_delete(_s);
	}
}

LPCSTR outfit_item_sect_names[] = {
	"burn_protection",
	"strike_protection",
	"shock_protection",
	"wound_protection",
	"radiation_protection",
	"telepatic_protection",
	"chemical_burn_protection",
	"explosion_protection",
	"fire_wound_protection",
};

LPCSTR outfit_item_param_names[] = {
	"ui_inv_outfit_burn_protection",			// "(burn_imm)",
	"ui_inv_outfit_strike_protection",			// "(strike_imm)",
	"ui_inv_outfit_shock_protection",			// "(shock_imm)",
	"ui_inv_outfit_wound_protection",			// "(wound_imm)",
	"ui_inv_outfit_radiation_protection",		// "(radiation_imm)",
	"ui_inv_outfit_telepatic_protection",		// "(telepatic_imm)",
	"ui_inv_outfit_chemical_burn_protection",	// "(chemical_burn_imm)",
	"ui_inv_outfit_explosion_protection",		// "(explosion_imm)",
	"ui_inv_outfit_fire_wound_protection",		// "(fire_wound_imm)",
};

void CUIOutfitParams::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base = "outfit_params";
	if (!xml_doc.NavigateToNode(_base, 0))	return;

	string256					_buff;
	CUIXmlInit::InitWindow(xml_doc, _base, 0, this);

	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		m_info_items[i] = xr_new<CUIStatic>();
		CUIStatic* _s = m_info_items[i];
		_s->SetAutoDelete(false);
		strconcat(sizeof(_buff), _buff, _base, ":static_", outfit_item_sect_names[i]);
		CUIXmlInit::InitStatic(xml_doc, _buff, 0, _s);
	}
}

bool CUIOutfitParams::Check(const shared_str& outfit_section)
{
	return pSettings->line_exist(outfit_section, "bones_koeff_protection");
}
#include "../string_table.h"
void CUIOutfitParams::SetInfo(const shared_str& outfit_section)
{

	string128					_buff;
	float						_h = 0.0f;
	DetachAll();
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];
		LPCSTR _sn = "%";

		float					_val;

		_val = pSettings->r_float(outfit_section, outfit_item_sect_names[i]);
		if (fsimilar(_val, 0.0f))				continue;
		_val *= 100.0f;

		LPCSTR _color = (_val > 0) ? "%c[green]" : "%c[red]";


		sprintf_s(_buff, "%s: %s %3.0f %s",
			CStringTable().translate(outfit_item_param_names[i]).c_str(),
			_color,
			_val,
			_sn);
		_s->SetText(_buff);
		_s->SetWndPos(_s->GetWndPos().x, _h);
		_h += _s->GetWndSize().y;
		AttachChild(_s);
	}
	SetHeight(_h);
}
