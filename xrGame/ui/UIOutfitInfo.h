#pragma once

#include "UIWindow.h"

class CUIScrollView;
class CCustomOutfit;
class CUIStatic;
class CUIXml;
struct SActorRestores;

class CUIOutfitInfo : public CUIWindow
{
CCustomOutfit*		m_outfit;
public:
					CUIOutfitInfo			();
	virtual			~CUIOutfitInfo			();

			void 	Update					(CCustomOutfit* outfit);	
			void 	InitFromXml				(CUIXml& xml_doc);
protected:
	void			SetItem_Protection		(u32 hitType, bool force_add);
	void			SetItem_Bonus			();

	CUIScrollView*	m_listWnd;

	enum{
		_item_start						= 0,
		_item_burn_immunity				= _item_start,
		_item_strike_immunity,
		_item_shock_immunity,
		_item_wound_immunity,		
		_item_radiation_immunity,
		_item_telepatic_immunity,
		_item_chemical_burn_immunity,
		_item_explosion_immunity,
		_item_fire_wound_immunity,

		_item_index_1,

		_item_health_restore_speed = _item_index_1,
		_item_psy_health_restore_speed,
		_item_radiation_restore_speed,
		_item_satiety_restore_speed,
		_item_power_restore_speed,
		_item_bleeding_restore_speed,
		_item_additional_inventory_weight,
		_item_power_loss,

		_max_item_index
	};
	float			GetRestoreByID(SActorRestores restores, u8 id);
	CUIStatic*		m_items[_max_item_index];
};