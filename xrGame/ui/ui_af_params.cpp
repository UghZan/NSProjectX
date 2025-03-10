#include "stdafx.h"
#include "ui_af_params.h"
#include "UIStatic.h"
#include "../object_broker.h"
#include "UIXmlInit.h"

CUIArtefactParams::CUIArtefactParams()
{
	Memory.mem_fill(m_info_items, 0, sizeof(m_info_items));
}

CUIArtefactParams::~CUIArtefactParams()
{
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];
		xr_delete(_s);
	}
}

LPCSTR af_item_sect_names[] = {
	"health_restore_speed",
	"psy_health_restore_speed",
	"radiation_restore_speed",
	"satiety_restore_speed",
	"power_restore_speed",
	"bleeding_restore_speed",
	"additional_inventory_weight",
	"additional_inventory_weight2",

	"burn_immunity",
	"strike_immunity",
	"shock_immunity",
	"wound_immunity",
	"radiation_immunity",
	"telepatic_immunity",
	"chemical_burn_immunity",
	"explosion_immunity",
	"fire_wound_immunity",
};

LPCSTR af_item_param_names[] = {
	"ui_inv_health",
	"ui_inv_psy_health",
	"ui_inv_radiation",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_additional_inventory_weight",
	"ui_inv_additional_inventory_weight2",

	"ui_inv_outfit_burn_protection",			// "(burn_imm)",
	"ui_inv_outfit_strike_protection",			// "(strike_imm)",
	"ui_inv_outfit_shock_protection",			// "(shock_imm)",
	"ui_inv_outfit_wound_protection",			// "(wound_imm)",
	"ui_inv_outfit_radiation_protection",		// "(radiation_imm)",
	"ui_inv_outfit_telepatic_protection",		// "(telepatic_imm)",
	"ui_inv_outfit_chemical_burn_protection",	// "(chemical_burn_imm)",
	"ui_inv_outfit_explosion_protection",		// "(explosion_imm)",
	"ui_inv_outfit_fire_wound_protection"		// "(fire_wound_imm)",
};
void CUIArtefactParams::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base = "af_params";
	if (!xml_doc.NavigateToNode(_base, 0))	return;

	string256					_buff;
	CUIXmlInit::InitWindow(xml_doc, _base, 0, this);

	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		strconcat(sizeof(_buff), _buff, _base, ":static_", af_item_sect_names[i]);
		if (!xml_doc.NavigateToNode(_buff, 0))	continue;

		m_info_items[i] = xr_new<CUIStatic>();
		CUIStatic* _s = m_info_items[i];
		_s->SetAutoDelete(false);
		CUIXmlInit::InitStatic(xml_doc, _buff, 0, _s);
	}
}

bool CUIArtefactParams::Check(const shared_str& af_section)
{
	return !!pSettings->line_exist(af_section, "af_actor_properties");
}
#include "../string_table.h"
void CUIArtefactParams::SetInfo(CArtefact* af)
{

	string128					_buff;
	float						_h = 0.0f;
	DetachAll();
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];
		if (!_s)
			continue;

		LPCSTR _sn = "%";

		float					_val = af->GetStatWithVariation((CArtefact::AFProperty)i);
		if (fis_zero(_val))				continue;
		if (i < _item_index1)
		{
			_sn = "%/s";
			if (i != _item_additional_inventory_weight && i != _item_additional_inventory_weight2)
			{
				_val *= 100.0f * 1 / ARTEFACTS_UPDATE_TIME;
			}
		}
		else
		{
			_val *= 100.0f;
		}


		LPCSTR _color = (_val > 0) ? "%c[green]" : "%c[red]";

		if (i == _item_bleeding_restore_speed)
			_val *= -1.0f;

		if (i == _item_bleeding_restore_speed || i == _item_radiation_restore_speed)
			_color = (_val > 0) ? "%c[red]" : "%c[green]";


		if (i != _item_additional_inventory_weight2)
		{
			sprintf_s(_buff, "%s %s %+.2f %s",
				CStringTable().translate(af_item_param_names[i]).c_str(),
				_color,
				_val,
				_sn);
			_s->SetText(_buff);
			_s->SetWndPos(_s->GetWndPos().x, _h);
			_h += _s->GetWndSize().y;
			AttachChild(_s);
		}
	}
	SetHeight(_h);
}
