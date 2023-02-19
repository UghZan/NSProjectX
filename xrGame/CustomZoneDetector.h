#include <CustomDetector.h>
#include <Level.h>
#include <Map_Manager.h>
#pragma once
class CZoneList : public CDetectList<CCustomZone>
{
protected:
	BOOL									feel_touch_contact(CObject* O);
	void									AddRemoveMapSpot(CCustomZone* pT, bool bAdd);
public:
	void									UpdateMapSpots(bool bAdd);
};

class CCustomZoneDetector : public CCustomDetector
{
	typedef	CCustomDetector	inherited;
protected:
	void UpdateDetector() override;
	void UpdateDetectList(Fvector3& pos, float radius) override
	{
		m_zone_list.feel_touch_update(pos, radius);
	};
	void LoadDetectList(LPCSTR sect) override
	{
		m_zone_list.load(sect, "zone");
	}
	void ClearDetectList() override
	{
		m_zone_list.clear();
	}
	void DestroyDetectList() override
	{
		m_zone_list.destroy();
	}
	void UpdateDetectList_MapSpots(bool mode) override
	{
		m_zone_list.UpdateMapSpots(mode);
	}
	CZoneList	m_zone_list;
};

