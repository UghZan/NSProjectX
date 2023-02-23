////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "eatable_item.h"
#include "xrmessages.h"
#include "../xrEngine/../xrNetServer/net_utils.h"
#include "physic_item.h"
#include "Level.h"
#include "entity_alive.h"
#include "EntityCondition.h"
#include "InventoryOwner.h"


CEatableItem::CEatableItem()
{
	m_fHealthInfluence = 0;
	m_fPsyHealthInfluence = 0;
	m_fPowerInfluence = 0;
	m_fSatietyInfluence = 0;
	m_fRadiationInfluence = 0;
	m_fWoundsHealPerc = 0;

	m_iUses = -1;

	m_physic_item	= 0;
}

CEatableItem::~CEatableItem()
{
}

DLL_Pure *CEatableItem::_construct	()
{
	m_physic_item	= smart_cast<CPhysicItem*>(this);
	return			(inherited::_construct());
}

void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);
	m_iMaxUses = pSettings->r_s32(section, "eat_portions_num");
	VERIFY(m_iPortionsNum < 10000);
}

BOOL CEatableItem::net_Spawn(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;

	m_iUses = m_iMaxUses;

	return TRUE;
};
bool CEatableItem::Useful() const
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if(Empty()) return false;

	return true;
}

void CEatableItem::OnH_B_Independent(bool just_before_destroy)
{
	if(!Useful()) 
	{
		object().setVisible(FALSE);
		object().setEnabled(FALSE);
		if (m_physic_item)
			m_physic_item->m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent(just_before_destroy);
}

void CEatableItem::UseBy(CEntityAlive* entity_alive)
{
	CInventoryOwner* IO = smart_cast<CInventoryOwner*>(entity_alive);
	R_ASSERT(IO);
	R_ASSERT(m_pCurrentInventory == IO->m_inventory);
	R_ASSERT(object().H_Parent()->ID() == entity_alive->ID());

	SMedicineInfluenceValues	V;
	V.Load(m_physic_item->cNameSect());

	entity_alive->conditions().ApplyInfluence(V, m_physic_item->cNameSect());

	//Log(m_physic_item->cNameSect().c_str());
	if (!fis_zero(READ_IF_EXISTS(pSettings, r_float, m_physic_item->cNameSect().c_str(), "boost_time", 0.0f)))
	{
		for (u8 i = 0; i < (u8)eBoostMaxCount; i++)
		{
			if (pSettings->line_exist(m_physic_item->cNameSect().c_str(), ef_boosters_section_names[i]))
			{
				float booster_value = pSettings->r_float(m_physic_item->cNameSect().c_str(), ef_boosters_section_names[i]);
				if (fis_zero(booster_value)) continue;
				SBooster B;
				B.Load(m_physic_item->cNameSect(), (EBoostParams)i);
				entity_alive->conditions().ApplyBooster(B, m_physic_item->cNameSect());
			}
		}
	}

	if (!IsGameTypeSingle() && OnServer())
	{
		NET_Packet				tmp_packet;
		CGameObject::u_EventGen(tmp_packet, GEG_PLAYER_USE_BOOSTER, entity_alive->ID());
		tmp_packet.w_u16(object_id());
		Level().Send(tmp_packet);
	}

	entity_alive->conditions().SetMaxPower(entity_alive->conditions().GetMaxPower() + m_fMaxPowerUpInfluence);

	//уменьшить количество порций
	if (m_iUses > 0)
		--(m_iUses);
	else
		m_iUses = 0;
}
