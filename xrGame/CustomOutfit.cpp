#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"
#include "inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"
#include "BoneProtections.h"


CCustomOutfit::CCustomOutfit()
{
	m_slot = OUTFIT_SLOT;

	m_flags.set(FUsingCondition, TRUE);

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for (int i = 0; i < ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;

	m_HealthRestoreSpeed = 0.0f;
	m_PsyRestoreSpeed = 0.0f;
	m_RadiationRestoreSpeed = 0.0f;
	m_SatietyRestoreSpeed = 0.0f;
	m_PowerRestoreSpeed = 0.0f;
	m_BleedingRestoreSpeed = 0.0f;

	m_boneProtection = xr_new<SBoneProtections>();
}

CCustomOutfit::~CCustomOutfit()
{
	xr_delete(m_boneProtection);
}

void CCustomOutfit::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
	P.w_float_q8(m_fCondition, 0.0f, 1.0f);
}

void CCustomOutfit::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
	P.r_float_q8(m_fCondition, 0.0f, 1.0f);
}

void CCustomOutfit::Load(LPCSTR section)
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn] = pSettings->r_float(section, "burn_protection");
	m_HitTypeProtection[ALife::eHitTypeStrike] = pSettings->r_float(section, "strike_protection");
	m_HitTypeProtection[ALife::eHitTypeShock] = pSettings->r_float(section, "shock_protection");
	m_HitTypeProtection[ALife::eHitTypeWound] = pSettings->r_float(section, "wound_protection");
	m_HitTypeProtection[ALife::eHitTypeRadiation] = pSettings->r_float(section, "radiation_protection");
	m_HitTypeProtection[ALife::eHitTypeTelepatic] = pSettings->r_float(section, "telepatic_protection");
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section, "chemical_burn_protection");
	m_HitTypeProtection[ALife::eHitTypeExplosion] = pSettings->r_float(section, "explosion_protection");
	m_HitTypeProtection[ALife::eHitTypeFireWound] = pSettings->r_float(section, "fire_wound_protection");
	m_HitTypeProtection[ALife::eHitTypePhysicStrike] = READ_IF_EXISTS(pSettings, r_float, section, "physic_strike_protection", 0.0f);
	m_hitFraction = READ_IF_EXISTS(pSettings, r_float, section, "hit_fraction", -1.0f);


	if (pSettings->line_exist(section, "actor_visual"))
		m_ActorVisual = pSettings->r_string(section, "actor_visual");
	else
		m_ActorVisual = NULL;

	m_ef_equipment_type = pSettings->r_u32(section, "ef_equipment_type");
	if (pSettings->line_exist(section, "power_loss"))
		m_fPowerLoss = pSettings->r_float(section, "power_loss");
	else
		m_fPowerLoss = 1.0f;

	m_HealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "health_restore_speed", 0.f);
	m_PsyRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "psy_health_restore_speed", 0.f);
	m_RadiationRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed", 0.f);
	m_SatietyRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "satiety_restore_speed", 0.f);
	m_PowerRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "power_restore_speed", 0.f);
	m_BleedingRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "bleeding_restore_speed", 0.f);

	m_additional_weight = pSettings->r_float(section, "additional_inventory_weight");
	m_additional_weight2 = pSettings->r_float(section, "additional_inventory_weight2");

	/*if (pSettings->line_exist(section, "nightvision_sect"))
		m_NightVisionSect = pSettings->r_string(section, "nightvision_sect");
	else
		m_NightVisionSect = NULL;*/

	m_full_icon_name = pSettings->r_string(section, "full_icon_name");
}

void CCustomOutfit::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= GetHitImmunity(hit_type);
	ChangeCondition(-hit_power);
}

float CCustomOutfit::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	return 1.0f - m_HitTypeProtection[hit_type] * GetCondition();
}

float CCustomOutfit::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	float fBase = m_HitTypeProtection[hit_type] * GetCondition();
	float bone = m_boneProtection->getBoneProtection(element);
	return 1.0f - fBase * bone;
}

float	CCustomOutfit::HitThruArmour(SHit* hit)
{
	float hit_power = hit->power;
#ifdef SIMPLIFIED_FIRE_WOUND_PROTECTION
	float protection = 1.0f - m_HitTypeProtection[ALife::eHitTypeFireWound] * GetCondition();
#else
	float protection = m_boneProtection->m_fHitFrac;
#endif

	float ap = hit->ap;
#ifdef SIMPLIFIED_FIRE_WOUND_PROTECTION
	float BoneArmor = m_HitTypeProtection[ALife::eHitTypeFireWound] * GetCondition();
#else
	float BoneArmor = m_boneProtection->getBoneArmour(ALife::eHitTypeFireWound) * GetCondition();
#endif

	if (!fis_zero(BoneArmor))
	{
		if (ap > BoneArmor)
		{
			//���� ������� �����
			if (!IsGameTypeSingle())
			{
				float hit_fraction = protection + (ap - BoneArmor) / ap;
				if (hit_fraction > 1.0f)
					hit_fraction = 1.0f;

				hit_power *= hit_fraction;
			}

			VERIFY(NewHitPower >= 0.0f);
		}
		else
		{
			//���� �� ������� �����
			hit_power *= protection;
			hit->add_wound = false; 	//���� ���
		}
	}
	return hit_power;
};

BOOL	CCustomOutfit::BonePassBullet(int boneID)
{
	return m_boneProtection->getBonePassBullet(s16(boneID));
};

#include "torch.h"
void	CCustomOutfit::OnMoveToSlot()
{
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner());
		if (pActor)
		{
			if (m_ActorVisual.size())
			{
				shared_str NewVisual = NULL;
				char* TeamSection = Game().getTeamSection(pActor->g_Team());
				if (TeamSection)
				{
					if (pSettings->line_exist(TeamSection, *cNameSect()))
					{
						NewVisual = pSettings->r_string(TeamSection, *cNameSect());
						string256 SkinName;
						strcpy_s(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
						strcat_s(SkinName, *NewVisual);
						strcat_s(SkinName, ".ogf");
						NewVisual._set(SkinName);
					}
				}

				if (!NewVisual.size())
					NewVisual = m_ActorVisual;

				pActor->ChangeVisual(NewVisual);
			}
			if (pSettings->line_exist(cNameSect(), "bones_koeff_protection")) {
				m_boneProtection->reload(pSettings->r_string(cNameSect(), "bones_koeff_protection"), smart_cast<CKinematics*>(pActor->Visual()));
				if (!fsimilar(m_hitFraction, -1.0f))
				{
					m_boneProtection->m_fHitFrac = m_hitFraction;
				}
			};
		}
	}
};

void	CCustomOutfit::OnMoveToRuck()
{
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner());
		if (pActor)
		{
			CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
			if (pTorch)
			{
				pTorch->SwitchNightVision(false);
			}
			if (m_ActorVisual.size())
			{
				shared_str DefVisual = pActor->GetDefaultVisualOutfit();
				if (DefVisual.size())
				{
					pActor->ChangeVisual(DefVisual);
				};
			}
		}
	}
};

u32	CCustomOutfit::ef_equipment_type() const
{
	return		(m_ef_equipment_type);
}

float CCustomOutfit::GetPowerLoss()
{
	if (m_fPowerLoss < 1 && GetCondition() <= 0)
	{
		return 1.0f;
	};
	return m_fPowerLoss;
};
