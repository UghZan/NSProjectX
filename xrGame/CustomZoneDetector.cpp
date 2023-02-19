#include "stdafx.h"
#include "CustomZoneDetector.h"


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

void CZoneList::AddRemoveMapSpot(CCustomZone* detectable, bool bAdd)
{
	if (m_ItemInfo.find(detectable) == m_ItemInfo.end())return;

	if (bAdd && !detectable->IsVisible()) return;

	CGameObject* pObj = smart_cast<CGameObject*>(detectable);

	ITEM_TYPE* zone_type = m_ItemInfo[detectable].curr_ref;
	if (xr_strlen(zone_type->item_map_location)) {
		if (bAdd)
			Level().MapManager().AddMapLocation(zone_type->item_map_location, pObj->ID());
		else
			Level().MapManager().RemoveMapLocation(zone_type->item_map_location, pObj->ID());
	}
}

void CZoneList::UpdateMapSpots(bool bAdd) // called on turn on/off only
{
	CZoneList::ItemMap_iter iter_start = m_ItemInfo.begin();
	CZoneList::ItemMap_iter iter_end = m_ItemInfo.end();

	for (CZoneList::ItemMap_iter it = iter_start; it != iter_end; ++it)
		AddRemoveMapSpot(it->first, bAdd);
}


void CCustomZoneDetector::UpdateDetector()
{
	if (!IsWorking()) return;
	if (!H_Parent()) return;

	if (!m_pCurrentActor) return;
	if (m_zone_list.m_ItemInfo.size() == 0)	return;

	CZoneList::ItemMap_iter iter_start = m_zone_list.m_ItemInfo.begin();
	CZoneList::ItemMap_iter iter_end = m_zone_list.m_ItemInfo.end();
	CZoneList::ItemMap_iter iter = iter_start;
	xr_map<CCustomZone*, ITEM_INFO> m_ZoneInfoMap = m_zone_list.m_ItemInfo;

	Fvector						detector_pos = H_Parent()->Position();
	float min_dist = flt_max;

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
	if (dist_to_zone < 0) dist_to_zone = 0;

	float fRelPow = dist_to_zone / m_fRadius;
	clamp(fRelPow, 0.f, 1.f);

	//определить текущую частоту срабатывания сигнала
	zone_info.cur_freq = item_type->min_freq +
		(item_type->max_freq - item_type->min_freq) * fRelPow * fRelPow;
	//Log("freq: ",zone_info.cur_freq);

	float snd_freq = m_minSoundPitch + (m_maxSoundPitch - m_minSoundPitch) * (1.0f - fRelPow);

	float corrected_snd_time = zone_info.cur_freq * 100.0f;

	if ((float)zone_info.snd_time > corrected_snd_time)
	{
		zone_info.snd_time = 0;
		HUD_SOUND::PlaySound(item_type->detect_snds, Fvector().set(0, 0, 0), this, true, false);
		if (item_type->detect_snds.m_activeSnd)
			item_type->detect_snds.m_activeSnd->snd.set_frequency(snd_freq);

	}
	else
		zone_info.snd_time += Device.dwTimeDelta;
}
