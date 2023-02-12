#pragma once
#include "UIWindow.h"

class CUIXml;
class CUIStatic;

class CUIItemParams :public CUIWindow
{
public:
	CUIItemParams();
	virtual						~CUIItemParams();
	void 						InitFromXml(CUIXml& xml_doc);
	bool 						Check(const shared_str& af_section);
	void 						SetInfo(const shared_str& af_section);

protected:
	enum {
		_item_start = 0,

		_item_eat_health = _item_start,
		_item_eat_satiety,
		_item_eat_power,
		_item_eat_radiation, 
		_item_eat_alcohol,
		_item_wounds_heal_perc,

		_max_item_index

	};
	CUIStatic* m_info_items[_max_item_index];
};