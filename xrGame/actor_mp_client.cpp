#include "stdafx.h"
#include "actor_mp_client.h"
#include "actorcondition.h"
#include <eatable_item.h>

CActorMP::CActorMP			()
{
	m_i_am_dead				= false;
}

void CActorMP::OnEvent(NET_Packet& P, u16 type)
{
	if (type == GEG_PLAYER_USE_BOOSTER)
	{
		use_booster(P);
		return;
	}
	inherited::OnEvent(P, type);
}

void CActorMP::use_booster(NET_Packet& packet)
{
	if (OnServer())
		return;

	u16 tmp_booster_id;
	packet.r_u16(tmp_booster_id);
	CObject* tmp_booster = Level().Objects.net_Find(tmp_booster_id);
	VERIFY2(tmp_booster, "using unknown or deleted booster");
	if (!tmp_booster)
	{
		Msg("! ERROR: trying to use unkown booster object, ID = %d", tmp_booster_id);
		return;
	}

	CEatableItem* tmp_eatable = smart_cast<CEatableItem*>(tmp_booster);
	VERIFY2(tmp_eatable, "using not eatable object");
	if (!tmp_eatable)
	{
		Msg("! ERROR: trying to use not eatable object, ID = %d", tmp_booster_id);
		return;
	}
	tmp_eatable->UseBy(this);
}

void CActorMP::Die			(CObject *killer)
{
	m_i_am_dead				= true;
	conditions().health()	= 0.f;
	inherited::Die			(killer);

	if(OnServer())
	{ //transfer all items to bag
	
	}
}
