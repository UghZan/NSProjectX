#pragma once
#include <CustomDetector.h>
#include <CustomMonster.h>
#include <Artifact.h>
#include <Actor.h>

class CMonsterList : public CDetectList<CCustomMonster>
{
protected:
	BOOL			feel_touch_contact(CObject* O);
	//maybe place the realisation in main class?
	void									AddRemoveMapSpot(CCustomMonster* pT, bool bAdd);
public:
	void									UpdateMapSpots(bool bAdd);
	CMonsterList() {};
};

class CCustomMonsterDetector :
	public CCustomDetector
{
	typedef	CCustomDetector	inherited;
protected:
	void UpdateDetector() override;
	void UpdateDetectList(Fvector3& pos, float radius) override
	{
		m_monster_list.feel_touch_update(pos, radius);
	};
	void LoadDetectList(LPCSTR sect) override
	{
		m_monster_list.load(sect, "monster");
	}
	void ClearDetectList() override
	{
		m_monster_list.clear();
	}
	void DestroyDetectList() override
	{
		m_monster_list.destroy();
	}
	void UpdateDetectList_MapSpots(bool mode) override
	{
		m_monster_list.UpdateMapSpots(mode);
	}
	CMonsterList	m_monster_list;
};

