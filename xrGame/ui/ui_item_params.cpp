#include "stdafx.h"
#include "ui_item_params.h"
#include "UIStatic.h"
#include "../object_broker.h"
#include "UIXmlInit.h"

CUIItemParams::CUIItemParams()
{
	Memory.mem_fill(m_info_items, 0, sizeof(m_info_items));
}

CUIItemParams::~CUIItemParams()
{
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];
		xr_delete(_s);
	}
}

LPCSTR item_sect_names[] = {
	"eat_health",
	"eat_satiety",
	"eat_power",
	"eat_radiation",
	"eat_alcohol",
	"wounds_heal_perc"
};

LPCSTR item_param_names[] = {
	"ui_inv_health",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_radiation",
	"ui_inv_alcohol",
	"ui_inv_bleeding"
};

LPCSTR ui_sect_names[] = {
	"item_health",
	"item_satiety",
	"item_power",
	"item_radiation",
	"item_alcohol",
	"item_bleeding"
};
void CUIItemParams::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base = "item_params";
	if (!xml_doc.NavigateToNode(_base, 0))	return;

	string256					_buff;
	CUIXmlInit::InitWindow(xml_doc, _base, 0, this);

	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		m_info_items[i] = xr_new<CUIStatic>();
		CUIStatic* _s = m_info_items[i];
		_s->SetAutoDelete(false);
		strconcat(sizeof(_buff), _buff, _base, ":static_", ui_sect_names[i]);
		CUIXmlInit::InitStatic(xml_doc, _buff, 0, _s);
	}
}

bool CUIItemParams::Check(const shared_str& item_section)
{
	return pSettings->line_exist(item_section, "eat_portions_num");
}
#include "../string_table.h"
void CUIItemParams::SetInfo(const shared_str& item_section)
{

	string128					_buff;
	float						_h = 0.0f;
	DetachAll();
	for (u32 i = _item_start; i < _max_item_index; ++i)
	{
		CUIStatic* _s = m_info_items[i];
		if (0 == pSettings->line_exist(item_section, item_sect_names[i])) continue;

		float					_val;
		_val = pSettings->r_float(item_section, item_sect_names[i]);

		if (fis_zero(_val))				continue;

		LPCSTR _sn = "%";

		LPCSTR _color = (_val > 0) ? "%c[green]" : "%c[red]";

		if (i == _item_wounds_heal_perc)
			_val *= -1.0f;

		if (i == _item_eat_alcohol || i == _item_eat_radiation)
			_color = (_val > 0) ? "%c[red]" : "%c[green]";


		sprintf_s(_buff, "%s %s %+.0f %s",
			CStringTable().translate(item_param_names[i]).c_str(),
			_color,
			_val * 100.0f,
			_sn);
		_s->SetText(_buff);
		_s->SetWndPos(_s->GetWndPos().x, _h);
		_h += _s->GetWndSize().y;
		AttachChild(_s);
	}
	SetHeight(_h);
}
