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
	public CInventoryItemObject
{
	typedef	CInventoryItemObject	inherited;
public:
	CCustomMonsterDetector(void);
	virtual ~CCustomMonsterDetector(void);

	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void Load(LPCSTR section);

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent(bool just_before_destroy);

	virtual void shedule_Update(u32 dt);
	virtual void UpdateCL();

	void TurnOn();
	void TurnOff();
	bool IsWorking() { return m_bWorking; }

	virtual void OnMoveToSlot();
	virtual void OnMoveToRuck();
	virtual void OnMoveToBelt();

protected:
	void StopAllSounds();
	void UpdateNightVisionMode();

	bool m_bWorking;

	float m_fRadius;
	float m_fPulseRadius;

	//если хоз€ин текущий актер
	CActor* m_pCurrentActor;
	CInventoryOwner* m_pCurrentInvOwner;

	CMonsterList m_monster_list;

	shared_str						m_nightvision_particle;

protected:
	u32					m_ef_detector_type;

public:
	virtual u32			ef_detector_type() const;
};

