#include "stdafx.h"
#include "customdetector.h"
#include "customzone.h"
#include "hudmanager.h"
#include "artifact.h"
#include "inventory.h"
#include "level.h"
#include "map_manager.h"
#include "cameraEffector.h"
#include "actor.h"
#include "ai_sounds.h"

CCustomDetector::CCustomDetector(void) 
{
	m_bWorking					= false;
}

CCustomDetector::~CCustomDetector(void) 
{
	m_zone_list.destroy();
	m_zone_list.clear();
}

BOOL CCustomDetector::net_Spawn(CSE_Abstract* DC) 
{
	m_pCurrentActor		 = NULL;
	m_pCurrentInvOwner	 = NULL;

	return		(inherited::net_Spawn(DC));
}

void CCustomDetector::Load(LPCSTR section) 
{
	inherited::Load			(section);

	m_fRadius				= pSettings->r_float(section,"radius");
	
	if( pSettings->line_exist(section,"night_vision_particle") )
		m_nightvision_particle	= pSettings->r_string(section,"night_vision_particle");

	m_zone_list.load(section, "zone");

	m_ef_detector_type	= pSettings->r_u32(section,"ef_detector_type");
}


void CCustomDetector::shedule_Update(u32 dt) 
{
	inherited::shedule_Update	(dt);
	
	if( !IsWorking() ) return;
	if( !H_Parent()  ) return;

	Position().set(H_Parent()->Position());

	if (H_Parent() && H_Parent() == Level().CurrentViewEntity())
	{
		Fvector					P; 
		P.set					(H_Parent()->Position());
		m_zone_list.feel_touch_update(P, m_fRadius);
		//UpdateNightVisionMode();
	}
}

void CCustomDetector::UpdateCL() 
{
	inherited::UpdateCL();

	if( !IsWorking() ) return;
	if( !H_Parent()  ) return;

	if(!m_pCurrentActor) return;
	if (m_zone_list.m_ItemInfo.size() == 0)	return;

	CZoneList::ItemMap_iter iter_start = m_zone_list.m_ItemInfo.begin();
	CZoneList::ItemMap_iter iter_end = m_zone_list.m_ItemInfo.end();
	CZoneList::ItemMap_iter iter = iter_start;
	xr_map<CCustomZone*, ITEM_INFO> m_ZoneInfoMap = m_zone_list.m_ItemInfo;

	Fvector						detector_pos = H_Parent()->Position();
	float min_dist				= flt_max;

	CCustomZone* detectable = iter_start->first;

	for (;iter_start != iter_end;++iter_start)//only nearest
	{
		detectable = iter_start->first;
		if (!detectable->IsVisible()) continue;

		float d = detector_pos.distance_to(detectable->Position());
		if (d < min_dist)
		{
			min_dist = d;
			iter = iter_start;
		}
	}

	ITEM_INFO& zone_info = iter->second;

	ITEM_TYPE* item_type = m_ZoneInfoMap[detectable].curr_ref;

	float dist_to_zone = min_dist;
	if(dist_to_zone<0) dist_to_zone = 0;
		
	float fRelPow = dist_to_zone / m_fRadius;
	clamp(fRelPow, 0.f, 1.f);

	//определить текущую частоту срабатывания сигнала
	zone_info.cur_freq = item_type->min_freq +
		(item_type->max_freq - item_type->min_freq) * fRelPow* fRelPow;
	//Log("freq: ",zone_info.cur_freq);

	float min_snd_freq = 0.85f;
	float max_snd_freq = 1.2f;
	float snd_freq = min_snd_freq + (max_snd_freq - min_snd_freq) * (1.0f - fRelPow);

	float corrected_snd_time = zone_info.cur_freq * 100.0f;
		
	if((float)zone_info.snd_time > corrected_snd_time)
	{
		zone_info.snd_time	= 0;
		HUD_SOUND::PlaySound	(item_type->detect_snds, Fvector().set(0,0,0), this, true, false);
		if (item_type->detect_snds.m_activeSnd)
			item_type->detect_snds.m_activeSnd->snd.set_frequency(snd_freq);

	} 
	else 
		zone_info.snd_time += Device.dwTimeDelta;
}

BOOL CZoneList::feel_touch_contact(CObject* O) 
{
	CCustomZone* zone = smart_cast<CCustomZone*>(O);
	if (!zone) return false;
	if (!zone->IsVisible()) return false;

	TypeMap_iter it = m_TypeMap.find(zone->DetectorID());
	if (it != m_TypeMap.end())
		return true;
	return false;
}

void CCustomDetector::OnH_A_Chield() 
{
	m_pCurrentActor				= smart_cast<CActor*>(H_Parent());
	m_pCurrentInvOwner			= smart_cast<CInventoryOwner*>(H_Parent());
	inherited::OnH_A_Chield		();
}

void CCustomDetector::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);
	
	m_pCurrentActor				= NULL;
	m_pCurrentInvOwner			= NULL;

	m_zone_list.clear();
}


u32	CCustomDetector::ef_detector_type	() const
{
	return	(m_ef_detector_type);
}

void CCustomDetector::OnMoveToRuck()
{
	inherited::OnMoveToRuck();
	TurnOff();
}

void CCustomDetector::OnMoveToSlot()
{
	inherited::OnMoveToSlot	();
	TurnOn					();
}

void CCustomDetector::OnMoveToBelt		()
{
	inherited::OnMoveToBelt	();
	TurnOn					();
}

void CCustomDetector::TurnOn()
{
	m_bWorking				= true;
	m_zone_list.UpdateMapSpots(IsWorking());
	UpdateNightVisionMode	();
}

void CCustomDetector::TurnOff() 
{
	m_bWorking				= false;
	m_zone_list.UpdateMapSpots(IsWorking());
	UpdateNightVisionMode	();
}

void CZoneList::AddRemoveMapSpot(CCustomZone* detectable, bool bAdd)
{
	if(m_ItemInfo.find(detectable) == m_ItemInfo.end() )return;

	if ( bAdd && !detectable->IsVisible()) return;
	
	CGameObject* pObj = smart_cast<CGameObject*>(detectable);

	ITEM_TYPE* zone_type = m_ItemInfo[detectable].curr_ref;
	if( xr_strlen(zone_type->item_map_location) ){
		if( bAdd )
			Level().MapManager().AddMapLocation(zone_type->item_map_location, pObj->ID());
		else
			Level().MapManager().RemoveMapLocation(zone_type->item_map_location, pObj->ID());
	}
}

void CZoneList::UpdateMapSpots(bool bAdd) // called on turn on/off only
{
	CZoneList::ItemMap_iter iter_start = m_ItemInfo.begin();
	CZoneList::ItemMap_iter iter_end = m_ItemInfo.end();

	for(CZoneList::ItemMap_iter it = iter_start; it != iter_end; ++it)
		AddRemoveMapSpot(it->first, bAdd);
}

#include "clsid_game.h"
#include "game_base_space.h"
void CCustomDetector::UpdateNightVisionMode()
{
//	CObject* tmp = Level().CurrentViewEntity();	
	/*bool bNightVision = false;
	if (GameID() == GAME_SINGLE)
	{
		bNightVision = Actor()->Cameras().GetPPEffector(EEffectorPPType(effNightvision))!=NULL;
	}
	else
	{
		if (Level().CurrentViewEntity() && 
			Level().CurrentViewEntity()->CLS_ID == CLSID_OBJECT_ACTOR)
		{
			CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
			if (pActor)
				bNightVision = pActor->Cameras().GetPPEffector(EEffectorPPType(effNightvision))!=NULL;
		}
	}

	bool bOn =	bNightVision && 
				m_pCurrentActor &&
				m_pCurrentActor==Level().CurrentViewEntity()&& 
				IsWorking() && 
				m_nightvision_particle.size();

	CDetectList::ItemMap_iter iter_start = m_zone_list.m_ItemInfo.begin();
	CDetectList::ItemMap_iter iter_end = m_zone_list.m_ItemInfo.end();
	for(CDetectList::ItemMap_iter it = iter_start; it != iter_end; ++it)
	{
		CCustomZone *pZone = it->first;
		ITEM_INFO& zone_info = it->second;

		if(bOn){
			Fvector zero_vector;
			zero_vector.set(0.f,0.f,0.f);

			if(!zone_info.pParticle)
				zone_info.pParticle = CParticlesObject::Create(*m_nightvision_particle,FALSE);
			
			zone_info.pParticle->UpdateParent(pZone->XFORM(),zero_vector);
			if(!zone_info.pParticle->IsPlaying())
				zone_info.pParticle->Play();
		}else{
			if(zone_info.pParticle){
				zone_info.pParticle->Stop			();
				CParticlesObject::Destroy(zone_info.pParticle);
			}
		}
	}*/
}
