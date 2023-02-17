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

	"health_restore_speed",
	"psy_health_restore_speed",
	"radiation_restore_speed",
	"satiety_restore_speed",
	"power_restore_speed",
	"bleeding_restore_speed",
	"additional_inventory_weight",
	"power_loss"
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

	"ui_inv_health",
	"ui_inv_psy_health",
	"ui_inv_radiation",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_additional_inventory_weight",
	"ui_inv_power_loss"
};

LPCSTR actor_param_names[] = {
	"satiety_health_v",
	"psy_health_v",
	"radiation_v",
	"satiety_v",
	"satiety_power_v",
	"wound_incarnation_v",
};

void CUIOutfitParams::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base = "outfit_params";
	if (!xml_doc.NavigateToNode(_base, 0))	return;
	string256					_buff;
	CUIXmlInit::InitWindow(xml_doc, _base, 0, this);

	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		strconcat(sizeof(_buff), _buff, _base, ":static_", outfit_item_sect_names[i]);
		if (!xml_doc.NavigateToNode(_buff, 0))	continue;

		m_info_items[i] = xr_new<CUIStatic>();
		CUIStatic* _s = m_info_items[i];
		_s->SetAutoDelete(false);
		CUIXmlInit::InitStatic(xml_doc, _buff, 0, _s);
	}
}

bool CUIOutfitParams::Check(const shared_str& outfit_section)
{
	return pSettings->line_exist(outfit_section, "bones_koeff_protection");
}

float CUIOutfitParams::GetOutfitStat(CCustomOutfit* outfit, u32 id)
{
	float ret = 0.0f;

	switch (id)
	{
	case _item_health_restore_speed:
		ret = outfit->m_HealthRestoreSpeed;
		break;
	case _item_psy_health_restore_speed:
		ret = outfit->m_PsyRestoreSpeed;
		break;
	case _item_radiation_restore_speed:
		ret = outfit->m_RadiationRestoreSpeed;
		break;
	case _item_satiety_restore_speed:
		ret = outfit->m_SatietyRestoreSpeed;
		break;
	case _item_power_restore_speed:
		ret = outfit->m_PowerRestoreSpeed;
		break;
	case _item_bleeding_restore_speed:
		ret = outfit->m_BleedingRestoreSpeed;
		break;
	case _item_additional_inventory_weight:
		ret = outfit->m_additional_weight;
		break;
	case _item_power_loss:
		ret = outfit->GetPowerLoss();
		break;
	}
	return ret;
}

#include "../string_table.h"
void CUIOutfitParams::SetInfo(CCustomOutfit* outfit)
{

	string128					_buff;
	float						_h = 0.0f;
	DetachAll();
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];

		if (!_s)
			continue;

		LPCSTR _sn = "";
		bool sign = false;

		float					_val = 0.0f;

		if (i < _item_index1)
		{
			_val = 1.0f - outfit->GetDefHitTypeProtection((ALife::EHitType)(ALife::eHitTypeBurn + i));
			if (fis_zero(_val))				continue;
		}
		else
		{
			_val = GetOutfitStat(outfit, i);
			if (fis_zero(_val))				continue;

			if (i != _item_additional_inventory_weight && i != _item_power_loss)
			{
				float _actor_val = pSettings->r_float("actor_condition", actor_param_names[ i - _item_index1]);
				_val = (_val / _actor_val);
			}
		}


		if (i != _item_radiation_restore_speed && i != _item_power_restore_speed && i != _item_additional_inventory_weight)
		{
			_val *= 100.0f;
			_sn = "%";
		}
		
		if (i == _item_additional_inventory_weight)
		{
			_sn = " kg.";
			sign = true;
		}

		LPCSTR _color = (_val > 0) ? "%c[green]" : "%c[red]";

		_color = (_val > 0) ? "%c[green]" : "%c[red]";

		if (i == _item_bleeding_restore_speed)
			_val *= -1.0f;

		if (i == _item_bleeding_restore_speed || i == _item_radiation_restore_speed)
			_color = (_val > 0) ? "%c[red]" : "%c[green]";


		if(sign)
			sprintf_s(_buff, "%s: %s %+3.0f %s",
			CStringTable().translate(outfit_item_param_names[i]).c_str(),
			_color,
			_val,
			_sn);
		else
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
