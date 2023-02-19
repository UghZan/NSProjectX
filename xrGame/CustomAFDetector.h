#pragma once
#include <CustomDetector.h>
#include <Artifact.h>
#include <Actor.h>

class CArtefactList : public CDetectList<CArtefact>
{
protected:
	virtual BOOL	feel_touch_contact(CObject* O);
	//maybe place the realisation in main class?
	void									AddRemoveMapSpot(CArtefact* pT, bool bAdd);
public:
	void									UpdateMapSpots(bool bAdd);
	CArtefactList() {};
};

class CCustomAFDetector :
	public CInventoryItemObject
{
	typedef	CInventoryItemObject	inherited;
public:
	CCustomAFDetector(void);
	virtual ~CCustomAFDetector(void);

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

	CArtefactList	m_af_list;

	shared_str						m_nightvision_particle;

protected:
	u32					m_ef_detector_type;

public:
	virtual u32			ef_detector_type() const;
};

