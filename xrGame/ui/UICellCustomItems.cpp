#include "stdafx.h"
#include "UICellCustomItems.h"
#include "UIInventoryUtilities.h"
#include "../Weapon.h"

#define INV_GRID_WIDTHF			50.0f
#define INV_GRID_HEIGHTF		50.0f

CUIInventoryCellItem::CUIInventoryCellItem(CInventoryItem* itm)
{
	m_pData											= (void*)itm;

	inherited::SetShader							(itm->m_icon_params.get_shader());

	m_grid_size.set									(itm->GetGridWidth(),itm->GetGridHeight());
	inherited::SetOriginalRect(itm->m_icon_params.original_rect());

	inherited::SetStretchTexture					(true);
}

bool CUIInventoryCellItem::EqualTo(CUICellItem* itm)
{
	CUIInventoryCellItem* ci = smart_cast<CUIInventoryCellItem*>(itm);
	if(!itm)				return false;
	return					(
								fsimilar(object()->GetCondition(), ci->object()->GetCondition(), 0.01f) &&
								(object()->object().cNameSect() == ci->object()->object().cNameSect())
							);
}


CUIAmmoCellItem::CUIAmmoCellItem(CWeaponAmmo* itm)
:inherited(itm)
{}

bool CUIAmmoCellItem::EqualTo(CUICellItem* itm)
{
	if(!inherited::EqualTo(itm))	return false;

	CUIAmmoCellItem* ci				= smart_cast<CUIAmmoCellItem*>(itm);
	if(!ci)							return false;

	return					( (object()->cNameSect() == ci->object()->cNameSect()) );
}

void CUIAmmoCellItem::Update()
{
	inherited::Update	();
	UpdateItemText		();
}

void CUIAmmoCellItem::UpdateItemText()
{
	if(NULL==m_custom_draw)
	{
		xr_vector<CUICellItem*>::iterator it = m_childs.begin();
		xr_vector<CUICellItem*>::iterator it_e = m_childs.end();
		
		u16 total				= object()->m_boxCurr;
		for(;it!=it_e;++it)
			total				= total + ((CUIAmmoCellItem*)(*it))->object()->m_boxCurr;

		string32				str;
		sprintf_s					(str,"%d",total);

		SetText					(str);
	}else
	{
		SetText					("");
	}
}


CUIWeaponCellItem::CUIWeaponCellItem(CWeapon* itm)
:inherited(itm)
{
	m_addons[eSilencer]		= NULL;
	m_addons[eScope]		= NULL;
	m_addons[eLauncher]		= NULL;

	if(itm->SilencerAttachable())
		m_addon_offset[eSilencer].set(object()->GetSilencerX(), object()->GetSilencerY());

	if(itm->ScopeAttachable())
		m_addon_offset[eScope].set(object()->GetScopeX(), object()->GetScopeY());

	if(itm->GrenadeLauncherAttachable())
		m_addon_offset[eLauncher].set(object()->GetGrenadeLauncherX(), object()->GetGrenadeLauncherY());
}

#include "../object_broker.h"
CUIWeaponCellItem::~CUIWeaponCellItem()
{
}

bool CUIWeaponCellItem::is_scope()
{
	return object()->ScopeAttachable()&&object()->IsScopeAttached();
}

bool CUIWeaponCellItem::is_silencer()
{
	return object()->SilencerAttachable()&&object()->IsSilencerAttached();
}

bool CUIWeaponCellItem::is_launcher()
{
	return object()->GrenadeLauncherAttachable()&&object()->IsGrenadeLauncherAttached();
}

void CUIWeaponCellItem::CreateIcon(eAddonType t, CIconParams params)
{
	m_addons[t]					= xr_new<CUIStatic>();	
	m_addons[t]->SetAutoDelete	(true);
	AttachChild					(m_addons[t]);
	m_addons[t]->SetShader(params.get_shader());
	m_addons[t]->SetOriginalRect(params.original_rect());
}

void CUIWeaponCellItem::DestroyIcon(eAddonType t)
{
	DetachChild		(m_addons[t]);
	m_addons[t]		= NULL;
}

CUIStatic* CUIWeaponCellItem::GetIcon(eAddonType t)
{
	return m_addons[t];
}

void CUIWeaponCellItem::Update()
{
	inherited::Update			();

	if (object()->SilencerAttachable()){
		if (object()->IsSilencerAttached())
		{
			if (!GetIcon(eSilencer))
			{
				CIconParams params(object()->GetSilencerName());
				CreateIcon(eSilencer, params);
				InitAddon(GetIcon(eSilencer), params, m_addon_offset[eSilencer]);
			}
		}
		else
		{
			if (m_addons[eSilencer])
				DestroyIcon(eSilencer);
		}
	}

	if (object()->ScopeAttachable()){
		if (object()->IsScopeAttached())
		{
			if (!GetIcon(eScope))
			{
				CIconParams params(object()->GetScopeName());
				CreateIcon(eScope, params);
				InitAddon(GetIcon(eScope), params, m_addon_offset[eScope]);
			}
		}
		else
		{
			if (m_addons[eScope])
				DestroyIcon(eScope);
		}
	}

	if (object()->GrenadeLauncherAttachable()){
		if (object()->IsGrenadeLauncherAttached())
		{
			if (!GetIcon(eLauncher))
			{
				CIconParams params(object()->GetGrenadeLauncherName());
				CreateIcon(eLauncher, params);
				InitAddon(GetIcon(eLauncher), params, m_addon_offset[eLauncher]);
			}
		}
		else
		{
			if (m_addons[eLauncher])
				DestroyIcon(eLauncher);
		}
	}
}

void CUIWeaponCellItem::OnAfterChild()
{
	if (is_silencer() && GetIcon(eSilencer))
		InitAddon(GetIcon(eSilencer), object()->GetSilencerName(), m_addon_offset[eSilencer]);

	if (is_scope() && GetIcon(eScope))
		InitAddon(GetIcon(eScope), object()->GetScopeName(), m_addon_offset[eScope]);

	if (is_launcher() && GetIcon(eLauncher))
		InitAddon(GetIcon(eLauncher), object()->GetGrenadeLauncherName(), m_addon_offset[eLauncher]);
}

void CUIWeaponCellItem::InitAddon(CUIStatic* s, CIconParams params, Fvector2 addon_offset)
{
	
		Frect					tex_rect;
		Fvector2				base_scale;
		base_scale.x			= GetWidth()/(INV_GRID_WIDTHF*m_grid_size.x);
		base_scale.y			= GetHeight()/(INV_GRID_HEIGHTF*m_grid_size.y);

		Fvector2				cell_size;

		Frect rect = params.original_rect();

		cell_size.x = rect.width();
		cell_size.y = rect.height();

		tex_rect.x1 = rect.x1;
		tex_rect.y1 = rect.y1;

		tex_rect.rb.add			(tex_rect.lt,cell_size);

		cell_size.mul			(base_scale);
		addon_offset.mul		(base_scale);

		s->SetWndSize			(cell_size);
		s->SetWndPos			(addon_offset);
		s->SetOriginalRect		(tex_rect);
		s->SetStretchTexture	(true);
}

CUIStatic* MakeAddonStatic(CUIDragItem* i, ref_shader& shader)
{
	CUIStatic* s = xr_new<CUIStatic>();
	s->SetShader(shader);
	s->SetAutoDelete(true);
	s->SetColor(i->wnd()->GetColor());
	i->wnd()->AttachChild(s);
	return s;
}

CUIDragItem* CUIWeaponCellItem::CreateDragItem()
{
	CUIDragItem* i = inherited::CreateDragItem();
	CUIStatic* s_silencer = GetIcon(eSilencer) ? MakeAddonStatic(i, GetShader()) : NULL;
	CUIStatic* s_scope = GetIcon(eScope) ? MakeAddonStatic(i, GetShader()) : NULL;
	CUIStatic* s_launcher = GetIcon(eLauncher) ? MakeAddonStatic(i, GetShader()) : NULL;

	return				i;
}

bool CUIWeaponCellItem::EqualTo(CUICellItem* itm)
{
	if(!inherited::EqualTo(itm))	return false;

	CUIWeaponCellItem* ci			= smart_cast<CUIWeaponCellItem*>(itm);
	if(!ci)							return false;

	bool b_addons					= ( (object()->GetAddonsState() == ci->object()->GetAddonsState()) );
	bool b_place					= ( (object()->m_eItemPlace == ci->object()->m_eItemPlace) );
	
	return							b_addons && b_place;
}

CBuyItemCustomDrawCell::CBuyItemCustomDrawCell	(LPCSTR str, CGameFont* pFont)
{
	m_pFont		= pFont;
	VERIFY		(xr_strlen(str)<16);
	strcpy		(m_string,str);
}

void CBuyItemCustomDrawCell::OnDraw(CUICellItem* cell)
{
	Fvector2							pos;
	cell->GetAbsolutePos				(pos);
	UI()->ClientToScreenScaled			(pos, pos.x, pos.y);
	m_pFont->Out						(pos.x, pos.y, m_string);
	m_pFont->OnRender					();
}
