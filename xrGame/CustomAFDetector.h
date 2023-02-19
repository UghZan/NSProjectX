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
	public CCustomDetector
{
	typedef	CCustomDetector	inherited;
protected:
	void UpdateDetector() override;
	void UpdateDetectList(Fvector3& pos, float radius) override
	{
		m_af_list.feel_touch_update(pos, radius);
	};
	void LoadDetectList(LPCSTR sect) override
	{
		m_af_list.load(sect, "af");
	}
	void ClearDetectList() override
	{
		m_af_list.clear();
	}
	void DestroyDetectList() override
	{
		m_af_list.destroy();
	}
	void UpdateDetectList_MapSpots(bool mode) override
	{
		m_af_list.UpdateMapSpots(mode);
	}

	CArtefactList	m_af_list;
};

