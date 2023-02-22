#include "stdafx.h"
#include "UIOutfitInfo.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UIScrollView.h"
#include "../actor.h"
#include <ActorCondition.h>
#include "../CustomOutfit.h"
#include "../string_table.h"

CUIOutfitInfo::CUIOutfitInfo()
{
	Memory.mem_fill			(m_items, 0, sizeof(m_items));
}

CUIOutfitInfo::~CUIOutfitInfo()
{
	for(u32 i=_item_start; i<_max_item_index; ++i)
	{
		CUIStatic* _s			= m_items[i];
		xr_delete				(_s);
	}
}

LPCSTR _imm_names []={
	"burn_immunity",
	"strike_immunity",
	"shock_immunity",
	"wound_immunity",		
	"radiation_immunity",
	"telepatic_immunity",
	"chemical_burn_immunity",
	"explosion_immunity",
	"fire_wound_immunity",

	"health_restore_speed",
	"psy_health_restore_speed",
	"radiation_restore_speed",
	"satiety_restore_speed",
	"power_restore_speed",
	"bleeding_restore_speed",
	"additional_inventory_weight",
	"power_loss"
};

LPCSTR outfit_actor_param_names[] = {
	"satiety_health_v",
	"psy_health_v",
	"radiation_v",
	"satiety_v",
	"satiety_power_v",
	"wound_incarnation_v",
};

LPCSTR _imm_st_names[]={
	"ui_inv_outfit_burn_protection",
	"ui_inv_outfit_shock_protection",
	"ui_inv_outfit_strike_protection",
	"ui_inv_outfit_wound_protection",
	"ui_inv_outfit_radiation_protection",
	"ui_inv_outfit_telepatic_protection",
	"ui_inv_outfit_chemical_burn_protection",
	"ui_inv_outfit_explosion_protection",
	"ui_inv_outfit_fire_wound_protection",

	"ui_inv_health",
	"ui_inv_psy_health",
	"ui_inv_radiation",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_additional_inventory_weight",
	"ui_inv_power_loss"
};

void CUIOutfitInfo::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base				= "outfit_info";

	string256					_buff;
	CUIXmlInit::InitWindow		(xml_doc, _base, 0, this);

	m_listWnd					= xr_new<CUIScrollView>(); m_listWnd->SetAutoDelete(true);
	AttachChild					(m_listWnd);
	strconcat					(sizeof(_buff),_buff, _base, ":scroll_view");
	CUIXmlInit::InitScrollView	(xml_doc, _buff, 0, m_listWnd);

	for(u32 i=_item_start; i< _max_item_index; ++i)
	{
		m_items[i]				= xr_new<CUIStatic>();
		CUIStatic* _s			= m_items[i];
		_s->SetAutoDelete		(false);
		strconcat				(sizeof(_buff),_buff, _base, ":static_", _imm_names[i]);
		CUIXmlInit::InitStatic	(xml_doc, _buff,	0, _s);
	}

}
#pragma optimize( "", off )
float CUIOutfitInfo::GetRestoreByID(SActorRestores& restores, u16 id)
{
	float ret = 0.0;
	switch (id)
	{
	case _item_health_restore_speed:
		ret = restores.HealthRestoreSpeed;
		break;
	case _item_psy_health_restore_speed:
		ret = restores.PsyRestoreSpeed;
		break;
	case _item_radiation_restore_speed:
		ret = restores.RadiationRestoreSpeed;
		break;
	case _item_satiety_restore_speed:
		ret = restores.SatietyRestoreSpeed;
		break;
	case _item_power_restore_speed:
		ret = restores.PowerRestoreSpeed;
		break;
	case _item_bleeding_restore_speed:
		ret = restores.BleedingRestoreSpeed;
		break;
	case _item_additional_inventory_weight:
		ret = m_outfit->m_additional_weight + Actor()->GetArtifactWeightBonus();
		break;
	case _item_power_loss:
		ret = m_outfit->GetPowerLoss();
		break;
	}

	return ret;
}
float CUIOutfitInfo::GetBoosterRestoreByID(CActorCondition& conditions, u16 id)
{
	switch (id)
	{
	case _item_health_restore_speed:
		return conditions.GetBoosterInfluence(eBoostHpRestore);
	case _item_psy_health_restore_speed:
		return conditions.GetBoosterInfluence(eBoostPsyHealthRestore);
	case _item_radiation_restore_speed:
		return conditions.GetBoosterInfluence(eBoostRadiationRestore);
	case _item_satiety_restore_speed:
		return conditions.GetBoosterInfluence(eBoostSatietyRestore);
	case _item_power_restore_speed:
		return conditions.GetBoosterInfluence(eBoostPowerRestore);
	case _item_bleeding_restore_speed:
		return conditions.GetBoosterInfluence(eBoostBleedingRestore);
	case _item_additional_inventory_weight:
		return conditions.GetBoosterInfluence(eBoostMaxWeight);
	}
}
//replaced my shitty implementation with OGSR' one

void CUIOutfitInfo::Update(CCustomOutfit* outfit)
{
	m_outfit				= outfit; 

	string128 _buff;

	auto artefactEffects = Actor()->GetActorStatRestores();
	CActorCondition conditions = Actor()->conditions();

	m_listWnd->Clear(); // clear existing items and do not scroll to top

	for (u16 i = _item_start; i < _max_item_index; i++)
	{
		string128 _buff;
		bool sign = true;

		CUIStatic* _s = m_items[i];
		if (!_s)
			continue;

		float _val = 0.0f,
			 _val_af = 0.0f,
			 _val_boost_protection = 0.0f,
			 _val_boost_immunity = 0.0f,
			 _val_boost = 0.0f;

		LPCSTR _sn = "%";

		//resistances
		if (i < _item_index_1)
		{
			sign = false;
			_val = m_outfit ? m_outfit->GetDefHitTypeProtection(ALife::EHitType(i)) : 1.0f;
			_val = 1.0f - _val;
			_val *= 100.0f;


			_val_af = Actor()->HitArtefactsOnBelt(1.0f, ALife::EHitType(i));
			_val_af = 1.0f - _val_af;
			_val_af *= 100.0f;

			//линейное уменьшение от бустеров
			_val_boost_protection = conditions.GetBoosterInfluence((EBoostParams)(eBoostBurnProtection + i)) * -100.0f;
			//процентное уменьшение от бустеров
			_val_boost_immunity = conditions.GetBoosterInfluence((EBoostParams)(eBoostBurnImmunity + i)) * 100.0f;
		}
		else //restores
		{
			_val = GetRestoreByID(artefactEffects, i);
			if (i < _item_additional_inventory_weight)
			{
				_val *= 100.0f * 1 / ARTEFACTS_UPDATE_TIME;
				_sn = "%/s";
			}
			if (i != _item_power_loss) _val_boost = GetBoosterRestoreByID(conditions, i) * -100.0f * 10.0f; //10 is accounting for ten updates per second
			else _val *= 100.0f;

			if (fis_zero(_val) && fis_zero(_val_boost))	continue;
		}

		if (fis_zero(_val) && fis_zero(_val_af) && fis_zero(_val_boost_immunity) && fis_zero(_val_boost_protection) && fis_zero(_val_boost))
		{
			continue;
		}

		if (i == _item_bleeding_restore_speed)
			_val *= -1.0f;

		LPCSTR _color = (_val > 0) ? "%c[green]" : "%c[red]";

		if (i == _item_bleeding_restore_speed || i == _item_radiation_restore_speed)
			_color = (_val > 0) ? "%c[red]" : "%c[green]";

		if (i == _item_power_loss)
		{
			sign = false;
			_color = (_val > 100) ? "%c[red]" : "%c[green]";
		}

		if (i == _item_additional_inventory_weight)
		{
			_sn = " kg.";
			_val_boost /= -1000.0f;
		}

		LPCSTR _imm_name = *CStringTable().translate(_imm_st_names[i]);

		int _sz = sprintf_s(_buff, sizeof(_buff), "%s: ", _imm_name);
		if (!fis_zero(_val))
		{
			if (sign) _sz += sprintf_s(_buff + _sz, sizeof(_buff) - _sz, "%s %+3.2f%s", _color, _val, _sn);
			else _sz += sprintf_s(_buff + _sz, sizeof(_buff) - _sz, "%s %3.2f%s", _color, _val, _sn);
		}

		if (!fis_zero(_val_af))
		{
			_sz += sprintf_s(_buff + _sz, sizeof(_buff) - _sz, "%s %+3.2f%%", (_val_af > 0.0f) ? "%c[green]" : "%c[red]", _val_af);
		}

		if (!fis_zero(_val_boost_immunity))
		{
			_sz += sprintf_s(_buff + _sz, sizeof(_buff) - _sz, "%s %+3.2f%%", (_val_boost_immunity > 0.0f) ? "%c[255,0,225,0]" : "%c[255,225,0,0]", _val_boost_immunity);
		}

		if (!fis_zero(_val_boost_protection))
		{
			_sz += sprintf_s(_buff + _sz, sizeof(_buff) - _sz, "%s(%+3.2f)", (_val_boost_protection > 0.0f) ? "%c[255,200,0,0]" : "%c[255,0,200,0]", _val_boost_protection);
		}

		if (!fis_zero(_val_boost))
		{
			_sz += sprintf_s(_buff + _sz, sizeof(_buff) - _sz, "%s %+3.2f%%/s", (_val_boost > 0.0f) ? "%c[255,200,0,0]" : "%c[255,0,200,0]", _val_boost);
		}

		_s->SetText(_buff);

		m_listWnd->AddWindow(_s, false);
	}


}
