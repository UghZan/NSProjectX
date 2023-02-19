#pragma once
#include "inventory_item_object.h"
#include "../xrEngine/feel_touch.h"
#include "hudsound.h"
#include "CustomZone.h"
#include "../ai_sounds.h"
#include "CDetectable.h"

class CCustomZone;
//описание типа зоны
struct ITEM_TYPE
{
	//интервал частот отыгрывани€ звука
	float		min_freq;
	float		max_freq;
	//звук реакции детектора на конкретную зону
	HUD_SOUND	detect_snds;

	shared_str	item_map_location;
};

//описание зоны, обнаруженной детектором
struct ITEM_INFO
{
	//ссылка на предмет, дл€ которого храним инфу
	ITEM_TYPE*						curr_ref;
	float							snd_time;
	//текуща€ частота работы датчика
	float							cur_freq;
	//particle for night-vision mode
	CParticlesObject*				pParticle;

	ITEM_INFO()
	{
		pParticle = NULL;
	};
	~ITEM_INFO()
	{
		if (pParticle)
			CParticlesObject::Destroy(pParticle);
	};
};

template<typename T>
class CDetectList : public Feel::Touch
{
protected:
	typedef xr_map<shared_str, ITEM_TYPE>	TypeMap;
	typedef typename TypeMap::iterator		TypeMap_iter;
	TypeMap									m_TypeMap;
	void									AddRemoveMapSpot(T* pT, bool bAdd);
public:
	bool									should_mark_map;

	typedef xr_map<T*, ITEM_INFO>			ItemMap;
	typedef typename ItemMap::iterator		ItemMap_iter;
	ItemMap									m_ItemInfo;
	void									UpdateMapSpots(bool bAdd);

protected:
	virtual void feel_touch_new(CObject* O)
	{
		T* ptr_T = smart_cast<T*>(O);
		R_ASSERT(ptr_T);
		CDetectable* det_T = smart_cast<CDetectable*>(O);
		R_ASSERT(det_T);
		TypeMap_iter	iter = m_TypeMap.find(det_T->DetectorID());
		R_ASSERT(iter != m_TypeMap.end());
		m_ItemInfo[ptr_T].snd_time = 0.0f;
		m_ItemInfo[ptr_T].curr_ref = &(iter->second);
	};

	virtual void feel_touch_delete(CObject* O)
	{
		T* ptr_T = smart_cast<T*>(O);
		R_ASSERT(ptr_T);
		m_ItemInfo.erase(ptr_T);
	}
public:
	virtual void load(LPCSTR section, LPCSTR prefix)
	{
		u32 i = 1, j = 1;
		string256 buff;
		do
		{
			sprintf_s(buff, "%s_class_%d", prefix, i);
			if (pSettings->line_exist(section, buff))
			{
				shared_str item_section = pSettings->r_string(section, buff);
				m_TypeMap.insert(std::make_pair(item_section, ITEM_TYPE()));
				ITEM_TYPE& item_type = m_TypeMap[item_section];

				sprintf_s(buff, "%s_min_freq_%d", prefix, i);
				item_type.min_freq = pSettings->r_float(section, buff);

				sprintf_s(buff, "%s_max_freq_%d", prefix, i);
				item_type.max_freq = pSettings->r_float(section, buff);

				sprintf_s(buff, "%s_sound_%d_", prefix, i);
				HUD_SOUND::LoadSound(section, buff, item_type.detect_snds, SOUND_TYPE_ITEM);

				++i;
			}
			else
				break;
		} while (true);
	}
	void			destroy()
	{
		TypeMap_iter it = m_TypeMap.begin();
		for (; it != m_TypeMap.end(); ++it)
			HUD_SOUND::DestroySound(it->second.detect_snds);
	}
	void			clear()
	{
		m_ItemInfo.clear();
		Feel::Touch::feel_touch.clear();
	}
};

class CZoneList : public CDetectList<CCustomZone>
{
protected:
	BOOL									feel_touch_contact(CObject* O);
	void									AddRemoveMapSpot(CCustomZone* pT, bool bAdd);
public:
	void									UpdateMapSpots(bool bAdd);
};

class CInventoryOwner;

class CCustomDetector :
	public CInventoryItemObject
{
	typedef	CInventoryItemObject	inherited;
public:
	CCustomDetector(void);
	virtual ~CCustomDetector(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	(bool just_before_destroy);

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();

	//virtual void feel_touch_new		(CObject* O);
	//virtual void feel_touch_delete	(CObject* O);
	//virtual BOOL feel_touch_contact	(CObject* O);

			void TurnOn				();
			void TurnOff			();
			bool IsWorking			() {return m_bWorking;}

	virtual void OnMoveToSlot		();
	virtual void OnMoveToRuck		();
	virtual void OnMoveToBelt		();

protected:
	void StopAllSounds				();
	//moved to CZoneList
	//void UpdateMapLocations			();

	//moved to CZoneList
	//void AddRemoveMapSpot			(CCustomZone* pZone, bool bAdd);
	void UpdateNightVisionMode		();

	bool m_bWorking;
	float m_fRadius;

	//если хоз€ин текущий актер
	CActor*				m_pCurrentActor;
	CInventoryOwner*	m_pCurrentInvOwner;

	CZoneList	m_zone_list;
	
	shared_str						m_nightvision_particle;

protected:
	u32					m_ef_detector_type;

public:
	virtual u32			ef_detector_type	() const;
};
