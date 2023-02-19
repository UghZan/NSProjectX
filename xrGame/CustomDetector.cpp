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
	DestroyDetectList();
	ClearDetectList();
}

BOOL CCustomDetector::net_Spawn(CSE_Abstract* DC) 
{
	m_pCurrentActor		 = NULL;
	m_pCurrentInvOwner	 = NULL;

	return		(inherited::net_Spawn(DC));
}

void CCustomDetector::UpdateCL()
{
	inherited::UpdateCL();

	UpdateDetector();
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

		UpdateDetectList(P, m_fRadius);
		//UpdateNightVisionMode();
	}
}


void CCustomDetector::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fRadius = pSettings->r_float(section, "radius");
	m_minSoundPitch = READ_IF_EXISTS(pSettings, r_float, section, "min_sound_pitch", 1.0f);
	m_maxSoundPitch = READ_IF_EXISTS(pSettings, r_float, section, "max_sound_pitch", 1.0f);

	if (pSettings->line_exist(section, "night_vision_particle"))
		m_nightvision_particle = pSettings->r_string(section, "night_vision_particle");

	LoadDetectList(section);

	m_ef_detector_type = pSettings->r_u32(section, "ef_detector_type");
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

	ClearDetectList();
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
	UpdateDetectList_MapSpots(IsWorking());
	UpdateNightVisionMode	();
}

void CCustomDetector::TurnOff() 
{
	m_bWorking				= false;
	UpdateDetectList_MapSpots(IsWorking());
	UpdateNightVisionMode	();
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
