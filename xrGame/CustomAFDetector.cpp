#include "stdafx.h"
#include "CustomAFDetector.h"

BOOL CArtefactList::feel_touch_contact(CObject* O) {
	CArtefact* art = smart_cast<CArtefact*>(O);
	if (!art) return false;
	if (!art->IsVisible()) return false;

	TypeMap_iter it = m_TypeMap.find(art->DetectorID());
	if (it != m_TypeMap.end())
		return true;
	return false;
}

void CCustomAFDetector::UpdateDetector()
{
	if (!IsWorking()) return;
	if (!H_Parent()) return;

	if (!m_pCurrentActor) return;
	if (m_af_list.m_ItemInfo.size() == 0)	return;

	CArtefactList::ItemMap_iter iter_start = m_af_list.m_ItemInfo.begin();
	CArtefactList::ItemMap_iter iter_end = m_af_list.m_ItemInfo.end();
	CArtefactList::ItemMap_iter iter = iter_start;
	xr_map<CArtefact*, ITEM_INFO> m_ZoneInfoMap = m_af_list.m_ItemInfo;

	Fvector						detector_pos = H_Parent()->Position();
	float min_dist = flt_max;

	CArtefact* detectable = iter_start->first;

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

void CArtefactList::AddRemoveMapSpot(CArtefact* pArt, bool bAdd)
{
	if (m_ItemInfo.find(pArt) == m_ItemInfo.end())return;

	ITEM_TYPE* art_type = m_ItemInfo[pArt].curr_ref;
	if (xr_strlen(art_type->item_map_location)) {
		if (bAdd)
			Level().MapManager().AddMapLocation(art_type->item_map_location, pArt->ID());
		else
			Level().MapManager().RemoveMapLocation(art_type->item_map_location, pArt->ID());
	}
}

void CArtefactList::UpdateMapSpots(bool bAdd) // called on turn on/off only
{
	CArtefactList::ItemMap_iter iter_start = m_ItemInfo.begin();
	CArtefactList::ItemMap_iter iter_end = m_ItemInfo.end();

	for (CArtefactList::ItemMap_iter it = iter_start; it != iter_end; ++it)
		AddRemoveMapSpot(it->first, bAdd);
}
