#pragma once

#include "../xrEngine/feel_touch.h"
#include "inventory_item_object.h"

#include "InfoPortionDefs.h"
#include "character_info_defs.h"

#include "script_export_space.h"

#include "PdaMsg.h"


class CInventoryOwner;
class CPda;

DEF_VECTOR (PDA_LIST, CPda*);

class CPda :
	public CInventoryItemObject,
	public Feel::Touch
{
	typedef	CInventoryItemObject inherited;
public:
											CPda					();
	virtual									~CPda					();

	virtual BOOL 							net_Spawn				(CSE_Abstract* DC);
	virtual void 							Load					(LPCSTR section);
	virtual void 							net_Destroy				();

	virtual void 							OnH_A_Chield			();
	virtual void 							OnH_B_Independent		(bool just_before_destroy);

	virtual void 							shedule_Update			(u32 dt);

	virtual void 							feel_touch_new			(CObject* O);
	virtual void 							feel_touch_delete		(CObject* O);
	virtual BOOL 							feel_touch_contact		(CObject* O);

	virtual u16								GetOriginalOwnerID		() {return m_idOriginalOwner;}
	virtual CInventoryOwner*				GetOriginalOwner		();
	virtual CObject*						GetOwnerObject			();


			void							TurnOn					() {m_bTurnedOff = false;}
			void							TurnOff					() {m_bTurnedOff = true;}
	
			bool 							IsActive				() {return IsOn();}
			bool 							IsOn					() {return !m_bTurnedOff;}
			bool 							IsOff					() {return m_bTurnedOff;}


			void							ActivePDAContacts		(xr_vector<CPda*>& res);
			CPda*							GetPdaFromOwner			(CObject* owner);
			u32								ActiveContactsNum		()							{return m_active_contacts.size();}


	virtual void							save					(NET_Packet &output_packet);
	virtual void							load					(IReader &input_packet);

	virtual LPCSTR							Name					();

	bool							get_hacked_status()
	{
		return m_bHacked;
	}

	void							set_hacked_status(bool new_status)
	{
		m_bHacked = new_status;
	}
protected:
	void									UpdateActiveContacts	();


	xr_vector<CObject*>						m_active_contacts;
	float									m_fRadius;

	u16										m_idOriginalOwner;
	shared_str					m_SpecificChracterOwner;
	xr_string								m_sFullName;

	bool									m_bHacked;

	bool									m_bTurnedOff;
	shared_str								m_scriptFunctionName;

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CPda)
#undef script_type_list
#define script_type_list save_type_list(CPda)
