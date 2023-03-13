#include "pch_script.h"
#include "UIInventoryWnd.h"
#include "../actor.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"
#include "../Artifact.h"
#include "../eatable_item.h"
#include "../BottleItem.h"
#include "../WeaponMagazined.h"
#include "../inventory.h"
#include "../game_base.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "UICellItem.h"
#include "UIListBoxItem.h"
#include "../CustomOutfit.h"
#include "../script_engine.h"
#include "../ai_space.h"
#include "../script_game_object.h"


void CUIInventoryWnd::EatItem(PIItem itm)
{
	SetCurrentItem							(NULL);
	if(!itm->Useful())						return;

	SendEvent_Item_Eat						(itm);

	PlaySnd									(eInvItemUse);
}

#include "../Medkit.h"
#include "../Antirad.h"
void CUIInventoryWnd::ActivatePropertiesBox()
{
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false;


	UIPropertiesBox.RemoveAll();

	CMedkit* pMedkit = smart_cast<CMedkit*>			(CurrentIItem());
	CAntirad* pAntirad = smart_cast<CAntirad*>			(CurrentIItem());
	CEatableItem* pEatableItem = smart_cast<CEatableItem*>		(CurrentIItem());
	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>	(CurrentIItem());
	CWeapon* pWeapon = smart_cast<CWeapon*>			(CurrentIItem());
	CScope* pScope = smart_cast<CScope*>			(CurrentIItem());
	CSilencer* pSilencer = smart_cast<CSilencer*>		(CurrentIItem());
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>	(CurrentIItem());
	CBottleItem* pBottleItem = smart_cast<CBottleItem*>		(CurrentIItem());

	bool	b_show = false;


	if (!pOutfit && CurrentIItem()->GetSlot() != NO_ACTIVE_SLOT && !m_pInv->m_slots[CurrentIItem()->GetSlot()].m_bPersistent && m_pInv->CanPutInSlot(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("st_move_to_slot", NULL, INVENTORY_TO_SLOT_ACTION);
		b_show = true;
	}
	if (CurrentIItem()->Belt() && m_pInv->CanPutInBelt(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("st_move_on_belt", NULL, INVENTORY_TO_BELT_ACTION);
		b_show = true;
	}

	if (CurrentIItem()->Ruck() && m_pInv->CanPutInRuck(CurrentIItem()) && (CurrentIItem()->GetSlot() == u32(-1) || !m_pInv->m_slots[CurrentIItem()->GetSlot()].m_bPersistent))
	{
		if (!pOutfit)
			UIPropertiesBox.AddItem("st_move_to_bag", NULL, INVENTORY_TO_BAG_ACTION);
		else
			UIPropertiesBox.AddItem("st_undress_outfit", NULL, INVENTORY_TO_BAG_ACTION);
		bAlreadyDressed = true;
		b_show = true;
	}
	if (pOutfit && !bAlreadyDressed)
	{
		UIPropertiesBox.AddItem("st_dress_outfit", NULL, INVENTORY_TO_SLOT_ACTION);
		b_show = true;
	}

	//отсоединение аддонов от вещи
	if (pWeapon)
	{
		if (pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			UIPropertiesBox.AddItem("st_detach_gl", NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
			b_show = true;
		}
		if (pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			UIPropertiesBox.AddItem("st_detach_scope", NULL, INVENTORY_DETACH_SCOPE_ADDON);
			b_show = true;
		}
		if (pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("st_detach_silencer", NULL, INVENTORY_DETACH_SILENCER_ADDON);
			b_show = true;
		}
		if (smart_cast<CWeaponMagazined*>(pWeapon) && IsGameTypeSingle())
		{
			bool b = (0 != pWeapon->GetAmmoElapsed());

			if (!b)
			{
				CUICellItem* itm = CurrentItem();
				for (u32 i = 0; i < itm->ChildsCount(); ++i)
				{
					pWeapon = smart_cast<CWeaponMagazined*>((CWeapon*)itm->Child(i)->m_pData);
					if (pWeapon->GetAmmoElapsed())
					{
						b = true;
						break;
					}
				}
			}

			if (b) {
				UIPropertiesBox.AddItem("st_unload_magazine", NULL, INVENTORY_UNLOAD_MAGAZINE);
				b_show = true;
			}
		}
	}

	//присоединение аддонов к активному слоту (2 или 3)
	if (pScope)
	{
		if (m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		{
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_pistol", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
		if (m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		{
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
	}
	else if (pSilencer)
	{
		if (m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
		{
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_pistol", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
		if (m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
		{
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
	}
	else if (pGrenadeLauncher)
	{
		if (m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		{
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_gl_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}

	}
	LPCSTR _action = NULL;

	if (pMedkit || pAntirad)
	{
		_action = "st_use";
	}
	else if (pEatableItem)
	{
		if (pBottleItem)
			_action = "st_drink";
		else
			_action = "st_eat";
	}

	if(_action){
			UIPropertiesBox.AddItem(_action,  NULL, INVENTORY_EAT_ACTION);
		b_show			= true;
	}

	CGameObject* gameObject = smart_cast<CGameObject*>(CurrentIItem());

	LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use1_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct1;
		if (ai().script_engine().functor(functor_name, funct1))
		{
			_action = funct1(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT2_ACTION);
				b_show = true;
			}
		}
	}

	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use2_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct2;
		if (ai().script_engine().functor(functor_name, funct2))
		{
			_action = funct2(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT3_ACTION);
				b_show = true;
			}
		}
	}

	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use3_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct3;
		if (ai().script_engine().functor(functor_name, funct3))
		{
			_action = funct3(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT4_ACTION);
				b_show = true;
			}
		}
	}

	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use4_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct4;
		if (ai().script_engine().functor(functor_name, funct4))
		{
			_action = funct4(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT5_ACTION);
				b_show = true;
			}
		}
	}

	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use5_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct5;
		if (ai().script_engine().functor(functor_name, funct5))
		{
			_action = funct5(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT6_ACTION);
				b_show = true;
			}
		}
	}

	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use6_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct6;
		if (ai().script_engine().functor(functor_name, funct6))
		{
			_action = funct6(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT7_ACTION);
				b_show = true;
			}
		}
	}
	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use7_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct7;
		if (ai().script_engine().functor(functor_name, funct7))
		{
			_action = funct7(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT8_ACTION);
				b_show = true;
			}
		}
	}
	functor_name = READ_IF_EXISTS(pSettings, r_string, gameObject->cNameSect(), "use8_functor", 0);
	if (functor_name)
	{
		luabind::functor<LPCSTR> funct8;
		if (ai().script_engine().functor(functor_name, funct8))
		{
			_action = funct8(gameObject->lua_game_object());
			if (_action)
			{
				UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT9_ACTION);
				b_show = true;
			}
		}
	}

	bool disallow_drop	= (pOutfit&&bAlreadyDressed);
	disallow_drop		|= !!CurrentIItem()->IsQuestItem();

	if(!disallow_drop)
	{

		UIPropertiesBox.AddItem("st_drop", NULL, INVENTORY_DROP_ACTION);
		b_show			= true;

		if(CurrentItem()->ChildsCount())
			UIPropertiesBox.AddItem("st_drop_all", (void*)33, INVENTORY_DROP_ACTION);
	}

	if(b_show)
	{
		UIPropertiesBox.AutoUpdateSize	();
		UIPropertiesBox.BringAllToTop	();

		Fvector2						cursor_pos;
		Frect							vis_rect;
		GetAbsoluteRect					(vis_rect);
		cursor_pos						= GetUICursor()->GetCursorPosition();
		cursor_pos.sub					(vis_rect.lt);
		UIPropertiesBox.Show			(vis_rect, cursor_pos);
		PlaySnd							(eInvProperties);
	}
}

void CUIInventoryWnd::ProcessPropertiesBoxClicked	()
{
	if(UIPropertiesBox.GetClickedItem())
	{
		switch(UIPropertiesBox.GetClickedItem()->GetTAG())
		{
		case INVENTORY_TO_SLOT_ACTION:	
			ToSlot(CurrentItem(), true);
			break;
		case INVENTORY_TO_BELT_ACTION:	
			ToBelt(CurrentItem(),false);
			break;
		case INVENTORY_TO_BAG_ACTION:	
			ToBag(CurrentItem(),false);
			break;
		case INVENTORY_DROP_ACTION:
			{
				void* d = UIPropertiesBox.GetClickedItem()->GetData();
				bool b_all = (d==(void*)33);

				DropCurrentItem(b_all);
			}break;
		case INVENTORY_EAT_ACTION:
			EatItem(CurrentIItem());
			break;
		case INVENTORY_EAT2_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use1_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct1;
				if (ai().script_engine().functor(functor_name, funct1))
				{
					if (funct1(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT3_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use2_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct2;
				if (ai().script_engine().functor(functor_name, funct2))
				{
					if (funct2(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT4_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use3_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct3;
				if (ai().script_engine().functor(functor_name, funct3))
				{
					if (funct3(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT5_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use4_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct4;
				if (ai().script_engine().functor(functor_name, funct4))
				{
					if (funct4(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT6_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use5_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct5;
				if (ai().script_engine().functor(functor_name, funct5))
				{
					if (funct5(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT7_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use6_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct6;
				if (ai().script_engine().functor(functor_name, funct6))
				{
					if (funct6(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT8_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use7_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct7;
				if (ai().script_engine().functor(functor_name, funct7))
				{
					if (funct7(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_EAT9_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(CurrentIItem());
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use8_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool> funct8;
				if (ai().script_engine().functor(functor_name, funct8))
				{
					if (funct8(GO->lua_game_object()))
						TryUseItem(CurrentIItem());
				}
			}
			break;
		}
		case INVENTORY_ATTACH_ADDON:
			AttachAddon((PIItem)(UIPropertiesBox.GetClickedItem()->GetData()));
			break;
		case INVENTORY_DETACH_SCOPE_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetScopeName());
			break;
		case INVENTORY_DETACH_SILENCER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetSilencerName());
			break;
		case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetGrenadeLauncherName());
			break;
		case INVENTORY_RELOAD_MAGAZINE:
			(smart_cast<CWeapon*>(CurrentIItem()))->Action(kWPN_RELOAD, CMD_START);
			break;
		case INVENTORY_UNLOAD_MAGAZINE:
			{
				CUICellItem * itm = CurrentItem();
				(smart_cast<CWeaponMagazined*>((CWeapon*)itm->m_pData))->UnloadMagazine();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					CUICellItem * child_itm			= itm->Child(i);
					(smart_cast<CWeaponMagazined*>((CWeapon*)child_itm->m_pData))->UnloadMagazine();
				}
			}break;
		}
	}
}

bool CUIInventoryWnd::TryUseItem(PIItem itm)
{
	CBottleItem*		pBottleItem			= smart_cast<CBottleItem*>		(itm);
	CMedkit*			pMedkit				= smart_cast<CMedkit*>			(itm);
	CAntirad*			pAntirad			= smart_cast<CAntirad*>			(itm);
	CEatableItem*		pEatableItem		= smart_cast<CEatableItem*>		(itm);

	if(!pMedkit && !pAntirad && !pEatableItem && !pBottleItem)
	{
		return false;
	}
	EatItem(itm);
	return true;
}

bool CUIInventoryWnd::DropItem(PIItem itm, CUIDragDropListEx* lst)
{
	if(lst==m_pUIOutfitList)
	{
		return TryUseItem			(itm);
/*
		CCustomOutfit*		pOutfit		= smart_cast<CCustomOutfit*>	(CurrentIItem());
		if(pOutfit)
			ToSlot			(CurrentItem(), true);
		else
			EatItem				(CurrentIItem());

		return				true;
*/
	}
	CUICellItem*	_citem	= lst->ItemsCount() ? lst->GetItemIdx(0) : NULL;
	PIItem _iitem	= _citem ? (PIItem)_citem->m_pData : NULL;

	if(!_iitem)						return	false;
	if(!_iitem->CanAttach(itm))		return	false;
	AttachAddon						(_iitem);

	return							true;
}
