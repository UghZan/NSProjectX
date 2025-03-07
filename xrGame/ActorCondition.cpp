#include "pch_script.h"
#include "actorcondition.h"
#include "actor.h"
#include "actorEffector.h"
#include "inventory.h"
#include "level.h"
#include "sleepeffector.h"
#include "game_base_space.h"
#include "autosave_manager.h"
#include "xrserver.h"
#include "ai_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "ui\UIVideoPlayerWnd.h"
#include "script_callback_ex.h"
#include "object_broker.h"
#include "weapon.h"

#define MAX_SATIETY					1.0f
#define START_SATIETY				0.5f

BOOL	GodMode	()	
{ 
	if (GameID() == GAME_SINGLE) 
		return psActorFlags.test(AF_GODMODE); 
	return FALSE;	
}

CActorCondition::CActorCondition(CActor *object) :
	inherited	(object)
{
	m_fJumpPower				= 0.f;
	m_fStandPower				= 0.f;
	m_fWalkPower				= 0.f;
	m_fJumpWeightPower			= 0.f;
	m_fWalkWeightPower			= 0.f;
	m_fOverweightWalkK			= 0.f;
	m_fOverweightJumpK			= 0.f;
	m_fAccelK					= 0.f;
	m_fSprintK					= 0.f;
	m_fAlcohol					= 0.f;
	m_fSatiety					= 1.0f;

	VERIFY						(object);
	m_object					= object;
	m_condition_flags.zero		();

}

CActorCondition::~CActorCondition(void)
{
}

void CActorCondition::LoadCondition(LPCSTR entity_section)
{
	inherited::LoadCondition(entity_section);

	LPCSTR						section = READ_IF_EXISTS(pSettings,r_string,entity_section,"condition_sect",entity_section);

	m_fJumpPower				= pSettings->r_float(section,"jump_power");
	m_fStandPower				= pSettings->r_float(section,"stand_power");
	m_fWalkPower				= pSettings->r_float(section,"walk_power");
	m_fJumpWeightPower			= pSettings->r_float(section,"jump_weight_power");
	m_fWalkWeightPower			= pSettings->r_float(section,"walk_weight_power");
	m_fOverweightWalkK			= pSettings->r_float(section,"overweight_walk_k");
	m_fOverweightJumpK			= pSettings->r_float(section,"overweight_jump_k");
	m_fAccelK					= pSettings->r_float(section,"accel_k");
	m_fSprintK					= pSettings->r_float(section,"sprint_k");

	//����� ���� � �������� ������ �������� ����� �������� �������
	m_fLimpingHealthBegin		= pSettings->r_float(section,	"limping_health_begin");
	m_fLimpingHealthEnd			= pSettings->r_float(section,	"limping_health_end");
	R_ASSERT					(m_fLimpingHealthBegin<=m_fLimpingHealthEnd);

	m_fLimpingPowerBegin		= pSettings->r_float(section,	"limping_power_begin");
	m_fLimpingPowerEnd			= pSettings->r_float(section,	"limping_power_end");
	R_ASSERT					(m_fLimpingPowerBegin<=m_fLimpingPowerEnd);

	m_fCantWalkPowerBegin		= pSettings->r_float(section,	"cant_walk_power_begin");
	m_fCantWalkPowerEnd			= pSettings->r_float(section,	"cant_walk_power_end");
	R_ASSERT					(m_fCantWalkPowerBegin<=m_fCantWalkPowerEnd);

	m_fCantSprintPowerBegin		= pSettings->r_float(section,	"cant_sprint_power_begin");
	m_fCantSprintPowerEnd		= pSettings->r_float(section,	"cant_sprint_power_end");
	R_ASSERT					(m_fCantSprintPowerBegin<=m_fCantSprintPowerEnd);

	m_fPowerLeakSpeed			= pSettings->r_float(section,"max_power_leak_speed");
	
	m_fV_Alcohol				= pSettings->r_float(section,"alcohol_v");

	m_fSatietyCritical			= pSettings->r_float(section,"satiety_critical");
	m_fV_Satiety				= pSettings->r_float(section,"satiety_v");		
	m_fV_SatietyPower			= pSettings->r_float(section,"satiety_power_v");
	m_fV_SatietyHealth			= pSettings->r_float(section,"satiety_health_v");
	
	m_MaxWalkWeight					= pSettings->r_float(section,"max_walk_weight");
}


//���������� ���������� � ����� �������
#include "UI.h"
#include "HUDManager.h"

void CActorCondition::UpdateCondition()
{
	UpdateSatiety();
	UpdateBoosters();

	if (GodMode())				return;
	if (!object().g_Alive())	return;
	if (!object().Local() && m_object != Level().CurrentViewEntity())		return;	
	
	float base_weight = object().MaxCarryWeight();
	float cur_weight = object().inventory().TotalWeight();

	if ((object().mstate_real&mcAnyMove)) {
		ConditionWalk(cur_weight / base_weight, isActorAccelerated(object().mstate_real,object().IsZoomAimingMode()), (object().mstate_real&mcSprint) != 0);
	}
	else {
		ConditionStand(cur_weight / base_weight);
	};
	
	if( IsGameTypeSingle() ){

		float k_max_power = 1.0f;

		if( true )
		{
/*
			CCustomOutfit* outfit	= m_object->GetOutfit();
			if(outfit)
				base_w += outfit->m_additional_weight2;
*/

			k_max_power = 1.0f + _min(cur_weight, base_weight)/ base_weight + _max(0.0f, (cur_weight - base_weight)/10.0f);
		}else
			k_max_power = 1.0f;
		
		SetMaxPower		(GetMaxPower() - m_fPowerLeakSpeed*m_fDeltaTime*k_max_power);
	}


	m_fAlcohol		+= m_fV_Alcohol*m_fDeltaTime;
	clamp			(m_fAlcohol,			0.0f,		1.0f);

	if ( IsGameTypeSingle() )
	{	
		CEffectorCam* ce = Actor()->Cameras().GetCamEffector((ECamEffectorType)effAlcohol);
		if	((m_fAlcohol>0.0001f) ){
			if(!ce){
				AddEffector(m_object,effAlcohol, "effector_alcohol", GET_KOEFF_FUNC(this, &CActorCondition::GetAlcohol));
			}
		}else{
			if(ce)
				RemoveEffector(m_object,effAlcohol);
		}

		
		CEffectorPP* ppe = object().Cameras().GetPPEffector((EEffectorPPType)effPsyHealth);
		
		string64			pp_sect_name;
		shared_str ln		= Level().name();
		strconcat			(sizeof(pp_sect_name),pp_sect_name, "effector_psy_health", "_", *ln);
		if(!pSettings->section_exist(pp_sect_name))
			strcpy_s			(pp_sect_name, "effector_psy_health");

		if	( !fsimilar(GetPsyHealth(), 1.0f, 0.05f) )
		{
			if(!ppe)
			{
				AddEffector(m_object,effPsyHealth, pp_sect_name, GET_KOEFF_FUNC(this, &CActorCondition::GetPsy));
			}
		}else
		{
			if(ppe)
				RemoveEffector(m_object,effPsyHealth);
		}
		if(fis_zero(GetPsyHealth()))
			health() =0.0f;
	};


	inherited::UpdateCondition	();

	if( IsGameTypeSingle() )
		UpdateTutorialThresholds();
}

void CActorCondition::UpdateBoosters()
{
	for (u8 i = 0;i < eBoostMaxCount;i++)
	{
		BOOSTER_MAP::iterator it = m_booster_influences.find((EBoostParams)i);
		if (it != m_booster_influences.end())
		{
			it->second.fBoostTime -= m_fDeltaTime / (IsGameTypeSingle() ? Level().GetGameTimeFactor() : 1.0f);
			if (it->second.fBoostTime <= 0.0f)
			{
				DisableBoostParameters(it->second);
				m_booster_influences.erase(it);
			}
		}
	}
}


void CActorCondition::UpdateSatiety()
{
	if (!IsGameTypeSingle()) return;

	float k = 1.0f;
	if(m_fSatiety>0)
	{
		m_fSatiety -=	m_fV_Satiety*
						k*
						m_fDeltaTime;
	
		clamp			(m_fSatiety,		0.0f,		1.0f);

	}
		
	float satiety_health_koef = (m_fSatiety - m_fSatietyCritical) / (m_fSatiety >= m_fSatietyCritical ? 1 - m_fSatietyCritical : m_fSatietyCritical);
	if (CanBeHarmed())
	{
		m_fDeltaHealth += m_fV_SatietyHealth * satiety_health_koef * m_fDeltaTime;
		m_fDeltaPower += m_fV_SatietyPower * (m_fV_SatietyPower > 0.0f ? m_fSatiety : 1.f) * m_fDeltaTime;
	}
}


CWound* CActorCondition::ConditionHit(SHit* pHDS)
{
	if (GodMode()) return NULL;
	return inherited::ConditionHit(pHDS);
}

//weight - "��������" ��� �� 0..1
void CActorCondition::ConditionJump(float weight)
{
	float power			=	m_fJumpPower;
	power				+=	m_fJumpWeightPower*weight*(weight>1.f?m_fOverweightJumpK:1.f);
	m_fPower			-=	HitPowerEffect(power);
}
void CActorCondition::ConditionWalk(float weight, bool accel, bool sprint)
{	
	float power			=	m_fWalkPower;
	power				+=	m_fWalkWeightPower*weight*(weight>1.f?m_fOverweightWalkK:1.f);
	power				*=	m_fDeltaTime*(accel?(sprint?m_fSprintK:m_fAccelK):1.f);
	m_fPower			-=	HitPowerEffect(power);
}

void CActorCondition::ConditionStand(float weight)
{	
	float power			= m_fStandPower;
	power				*= m_fDeltaTime;
	m_fPower			-= power;
}


bool CActorCondition::IsCantWalk() const
{
	if(m_fPower< m_fCantWalkPowerBegin)
		m_bCantWalk		= true;
	else if(m_fPower > m_fCantWalkPowerEnd)
		m_bCantWalk		= false;
	return				m_bCantWalk;
}

#include "CustomOutfit.h"

bool CActorCondition::IsCantWalkWeight()
{
	if(IsGameTypeSingle() && !GodMode())
	{
		float max_w				= m_MaxWalkWeight;

		CCustomOutfit* outfit	= m_object->GetOutfit();
		if(outfit)
			max_w += outfit->m_additional_weight2;

		max_w += object().GetArtifactWeightBonus();

		if( object().inventory().TotalWeight() > max_w )
		{
			m_condition_flags.set			(eCantWalkWeight, TRUE);
			return true;
		}
	}
	m_condition_flags.set					(eCantWalkWeight, FALSE);
	return false;
}

bool CActorCondition::IsCantSprint() const
{
	if(m_fPower< m_fCantSprintPowerBegin)
		m_bCantSprint	= true;
	else if(m_fPower > m_fCantSprintPowerEnd)
		m_bCantSprint	= false;
	return				m_bCantSprint;
}

bool CActorCondition::IsLimping() const
{
	if(m_fPower< m_fLimpingPowerBegin || GetHealth() < m_fLimpingHealthBegin)
		m_bLimping = true;
	else if(m_fPower > m_fLimpingPowerEnd && GetHealth() > m_fLimpingHealthEnd)
		m_bLimping = false;
	return m_bLimping;
}
extern bool g_bShowHudInfo;

void CActorCondition::save(NET_Packet &output_packet)
{
	inherited::save		(output_packet);
	save_data			(m_fAlcohol, output_packet);
	save_data			(m_condition_flags, output_packet);
	save_data			(m_fSatiety, output_packet);

	save_data(m_curr_medicine_influence.fHealth, output_packet);
	save_data(m_curr_medicine_influence.fPsyHealth, output_packet);
	save_data(m_curr_medicine_influence.fPower, output_packet);
	save_data(m_curr_medicine_influence.fSatiety, output_packet);
	save_data(m_curr_medicine_influence.fRadiation, output_packet);
	save_data(m_curr_medicine_influence.fWoundsHeal, output_packet);
	save_data(m_curr_medicine_influence.fMaxPowerUp, output_packet);
	save_data(m_curr_medicine_influence.fAlcohol, output_packet);
	save_data(m_curr_medicine_influence.fTimeTotal, output_packet);
	save_data(m_curr_medicine_influence.fTimeCurrent, output_packet);

	output_packet.w_u8((u8)m_booster_influences.size());
	BOOSTER_MAP::iterator b = m_booster_influences.begin(), e = m_booster_influences.end();
	for (; b != e; b++)
	{
		output_packet.w_u8((u8)b->second.m_type);
		output_packet.w_float(b->second.fBoostValue);
		output_packet.w_float(b->second.fBoostTime);
	}
}

void CActorCondition::load(IReader &input_packet)
{
	inherited::load		(input_packet);
	load_data			(m_fAlcohol, input_packet);
	load_data			(m_condition_flags, input_packet);
	load_data			(m_fSatiety, input_packet);

	load_data(m_curr_medicine_influence.fHealth, input_packet);
	load_data(m_curr_medicine_influence.fPsyHealth, input_packet);
	load_data(m_curr_medicine_influence.fPower, input_packet);
	load_data(m_curr_medicine_influence.fSatiety, input_packet);
	load_data(m_curr_medicine_influence.fRadiation, input_packet);
	load_data(m_curr_medicine_influence.fWoundsHeal, input_packet);
	load_data(m_curr_medicine_influence.fMaxPowerUp, input_packet);
	load_data(m_curr_medicine_influence.fAlcohol, input_packet);
	load_data(m_curr_medicine_influence.fTimeTotal, input_packet);
	load_data(m_curr_medicine_influence.fTimeCurrent, input_packet);

	u8 cntr = input_packet.r_u8();
	for (; cntr > 0; cntr--)
	{
		SBooster B;
		B.m_type = (EBoostParams)input_packet.r_u8();
		B.fBoostValue = input_packet.r_float();
		B.fBoostTime = input_packet.r_float();
		m_booster_influences[B.m_type] = B;
		BoostParameters(B);
	}
}

void CActorCondition::reinit	()
{
	inherited::reinit	();
	m_bLimping					= false;
	m_fSatiety					= 1.f;
}

void CActorCondition::ChangeAlcohol	(float value)
{
	m_fAlcohol += value;
}

void CActorCondition::ChangeSatiety(float value)
{
	m_fSatiety += value;
	clamp		(m_fSatiety, 0.0f, 1.0f);
}

void CActorCondition::BoostParameters(const SBooster& B)
{
	if (OnServer())
	{
		switch (B.m_type)
		{
		case eBoostHpRestore: BoostHpRestore(B.fBoostValue); break;
		case eBoostPsyHealthRestore: BoostPsyRestore(B.fBoostValue); break;
		case eBoostRadiationRestore: BoostRadiationRestore(B.fBoostValue); break;
		case eBoostSatietyRestore: BoostSatietyRestore(B.fBoostValue); break;
		case eBoostPowerRestore: BoostPowerRestore(B.fBoostValue); break;
		case eBoostBleedingRestore: BoostBleedingRestore(B.fBoostValue); break;
		case eBoostAlcoholRestore: BoostAlcoholRestore(B.fBoostValue); break;

		case eBoostMaxWeight: BoostMaxWeight(B.fBoostValue); break;

		case eBoostBurnImmunity: BoostBurnImmunity(B.fBoostValue); break;
		case eBoostShockImmunity: BoostShockImmunity(B.fBoostValue); break;
		case eBoostRadiationImmunity: BoostRadiationImmunity(B.fBoostValue); break;
		case eBoostTelepaticImmunity: BoostTelepaticImmunity(B.fBoostValue); break;
		case eBoostChemicalBurnImmunity: BoostChemicalBurnImmunity(B.fBoostValue); break;
		case eBoostExplImmunity: BoostExplImmunity(B.fBoostValue); break;
		case eBoostStrikeImmunity: BoostStrikeImmunity(B.fBoostValue); break;
		case eBoostFireWoundImmunity: BoostFireWoundImmunity(B.fBoostValue); break;
		case eBoostWoundImmunity: BoostWoundImmunity(B.fBoostValue); break;

		case eBoostBurnProtection: BoostBurnProtection(B.fBoostValue); break;
		case eBoostShockProtection: BoostShockProtection(B.fBoostValue); break;
		case eBoostRadiationProtection: BoostRadiationProtection(B.fBoostValue); break;
		case eBoostTelepaticProtection: BoostTelepaticProtection(B.fBoostValue); break;
		case eBoostChemicalBurnProtection: BoostChemicalBurnProtection(B.fBoostValue); break;
		case eBoostExplProtection: BoostExplProtection(B.fBoostValue); break;
		case eBoostStrikeProtection: BoostStrikeProtection(B.fBoostValue); break;
		case eBoostFireWoundProtection: BoostFireWoundProtection(B.fBoostValue); break;
		case eBoostWoundProtection: BoostWoundProtection(B.fBoostValue); break;
		default: NODEFAULT;
		}
	}
}

void CActorCondition::DisableBoostParameters(const SBooster& B)
{
	if (!OnServer())
		return;

	switch (B.m_type)
	{
		case eBoostHpRestore: BoostHpRestore(-B.fBoostValue); break;
		case eBoostPsyHealthRestore: BoostPsyRestore(-B.fBoostValue); break;
		case eBoostRadiationRestore: BoostRadiationRestore(-B.fBoostValue); break;
		case eBoostSatietyRestore: BoostSatietyRestore(-B.fBoostValue); break;
		case eBoostPowerRestore: BoostPowerRestore(-B.fBoostValue); break;
		case eBoostBleedingRestore: BoostBleedingRestore(-B.fBoostValue); break;
		case eBoostAlcoholRestore: BoostAlcoholRestore(-B.fBoostValue); break;

		case eBoostMaxWeight: BoostMaxWeight(-B.fBoostValue); break;

		case eBoostBurnImmunity: BoostBurnImmunity(-B.fBoostValue); break;
		case eBoostShockImmunity: BoostShockImmunity(-B.fBoostValue); break;
		case eBoostRadiationImmunity: BoostRadiationImmunity(-B.fBoostValue); break;
		case eBoostTelepaticImmunity: BoostTelepaticImmunity(-B.fBoostValue); break;
		case eBoostChemicalBurnImmunity: BoostChemicalBurnImmunity(-B.fBoostValue); break;
		case eBoostExplImmunity: BoostExplImmunity(-B.fBoostValue); break;
		case eBoostStrikeImmunity: BoostStrikeImmunity(-B.fBoostValue); break;
		case eBoostFireWoundImmunity: BoostFireWoundImmunity(-B.fBoostValue); break;
		case eBoostWoundImmunity: BoostWoundImmunity(-B.fBoostValue); break;

		case eBoostBurnProtection: BoostBurnProtection(-B.fBoostValue); break;
		case eBoostShockProtection: BoostShockProtection(-B.fBoostValue); break;
		case eBoostRadiationProtection: BoostRadiationProtection(-B.fBoostValue); break;
		case eBoostTelepaticProtection: BoostTelepaticProtection(-B.fBoostValue); break;
		case eBoostChemicalBurnProtection: BoostChemicalBurnProtection(-B.fBoostValue); break;
		case eBoostExplProtection: BoostExplProtection(-B.fBoostValue); break;
		case eBoostStrikeProtection: BoostStrikeProtection(-B.fBoostValue); break;
		case eBoostFireWoundProtection: BoostFireWoundProtection(-B.fBoostValue); break;
		case eBoostWoundProtection: BoostWoundProtection(-B.fBoostValue); break;
		default: NODEFAULT;
	}
}

#pragma region Boost Restores
void CActorCondition::BoostHpRestore(const float value)
{
	m_change_v.m_fV_HealthRestore += value;
}
void CActorCondition::BoostPsyRestore(const float value)
{
	m_change_v.m_fV_PsyHealth += value;
}
void CActorCondition::BoostSatietyRestore(const float value)
{
	m_fV_Satiety += value;
}
void CActorCondition::BoostPowerRestore(const float value)
{
	m_fV_SatietyPower += value;
}
void CActorCondition::BoostRadiationRestore(const float value)
{
	m_change_v.m_fV_Radiation += value;
}
void CActorCondition::BoostBleedingRestore(const float value)
{
	m_change_v.m_fV_WoundIncarnation += value;
}
void CActorCondition::BoostAlcoholRestore(const float value)
{
	m_fV_Alcohol += value;
}
#pragma endregion

#pragma region Boost Immunities + Max Weight
void CActorCondition::BoostMaxWeight(const float value)
{
	m_object->inventory().SetMaxWeight(object().inventory().GetMaxWeight() + value);
	m_MaxWalkWeight += value;
}
void CActorCondition::BoostBurnImmunity(const float value)
{
	m_fBoostBurnImmunity += value;
}
void CActorCondition::BoostShockImmunity(const float value)
{
	m_fBoostShockImmunity += value;
}
void CActorCondition::BoostRadiationImmunity(const float value)
{
	m_fBoostRadiationImmunity += value;
}
void CActorCondition::BoostTelepaticImmunity(const float value)
{
	m_fBoostTelepaticImmunity += value;
}
void CActorCondition::BoostChemicalBurnImmunity(const float value)
{
	m_fBoostChemicalBurnImmunity += value;
}
void CActorCondition::BoostExplImmunity(const float value)
{
	m_fBoostExplImmunity += value;
}
void CActorCondition::BoostStrikeImmunity(const float value)
{
	m_fBoostStrikeImmunity += value;
}
void CActorCondition::BoostFireWoundImmunity(const float value)
{
	m_fBoostFireWoundImmunity += value;
}
void CActorCondition::BoostWoundImmunity(const float value)
{
	m_fBoostWoundImmunity += value;
}
#pragma endregion

#pragma region Boost Protections
void CActorCondition::BoostBurnProtection(const float value)
{
	m_fBoostBurnProtection += value;
}
void CActorCondition::BoostShockProtection(const float value)
{
	m_fBoostShockProtection += value;
}
void CActorCondition::BoostRadiationProtection(const float value)
{
	m_fBoostRadiationProtection += value;
}
void CActorCondition::BoostTelepaticProtection(const float value)
{
	m_fBoostTelepaticProtection += value;
}
void CActorCondition::BoostChemicalBurnProtection(const float value)
{
	m_fBoostChemicalBurnProtection += value;
}
void CActorCondition::BoostExplProtection(const float value)
{
	m_fBoostExplProtection += value;
}
void CActorCondition::BoostStrikeProtection(const float value)
{
	m_fBoostStrikeProtection += value;
}
void CActorCondition::BoostFireWoundProtection(const float value)
{
	m_fBoostFireWoundProtection += value;
}
void CActorCondition::BoostWoundProtection(const float value)
{
	m_fBoostWoundProtection += value;
}
#pragma endregion

void CActorCondition::UpdateTutorialThresholds()
{
	string256						cb_name;
	static float _cPowerThr			= pSettings->r_float("tutorial_conditions_thresholds","power");
	static float _cPowerMaxThr		= pSettings->r_float("tutorial_conditions_thresholds","max_power");
	static float _cBleeding			= pSettings->r_float("tutorial_conditions_thresholds","bleeding");
	static float _cSatiety			= pSettings->r_float("tutorial_conditions_thresholds","satiety");
	static float _cRadiation		= pSettings->r_float("tutorial_conditions_thresholds","radiation");
	static float _cWpnCondition		= pSettings->r_float("tutorial_conditions_thresholds","weapon_jammed");
	static float _cPsyHealthThr		= pSettings->r_float("tutorial_conditions_thresholds","psy_health");



	bool b = true;
	if(b && !m_condition_flags.test(eCriticalPowerReached) && GetPower()<_cPowerThr){
		m_condition_flags.set			(eCriticalPowerReached, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_critical_power");
	}

	if(b && !m_condition_flags.test(eCriticalMaxPowerReached) && GetMaxPower()<_cPowerMaxThr){
		m_condition_flags.set			(eCriticalMaxPowerReached, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_critical_max_power");
	}

	if(b && !m_condition_flags.test(eCriticalBleedingSpeed) && BleedingSpeed()>_cBleeding){
		m_condition_flags.set			(eCriticalBleedingSpeed, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_bleeding");
	}

	if(b && !m_condition_flags.test(eCriticalSatietyReached) && GetSatiety()<_cSatiety){
		m_condition_flags.set			(eCriticalSatietyReached, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_satiety");
	}

	if(b && !m_condition_flags.test(eCriticalRadiationReached) && GetRadiation()>_cRadiation){
		m_condition_flags.set			(eCriticalRadiationReached, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_radiation");
	}

	if(b && !m_condition_flags.test(ePhyHealthMinReached) && GetPsyHealth()>_cPsyHealthThr){
//.		m_condition_flags.set			(ePhyHealthMinReached, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_psy");
	}

	if(b && !m_condition_flags.test(eCantWalkWeight)){
//.		m_condition_flags.set			(eCantWalkWeight, TRUE);
		b=false;
		strcpy_s(cb_name,"_G.on_actor_cant_walk_weight");
	}

	if(b && !m_condition_flags.test(eWeaponJammedReached)&&m_object->inventory().GetActiveSlot()!=NO_ACTIVE_SLOT){
		PIItem item							= m_object->inventory().ItemFromSlot(m_object->inventory().GetActiveSlot());
		CWeapon* pWeapon					= smart_cast<CWeapon*>(item); 
		if(pWeapon&&pWeapon->GetCondition()<_cWpnCondition){
			m_condition_flags.set			(eWeaponJammedReached, TRUE);b=false;
			strcpy_s(cb_name,"_G.on_actor_weapon_jammed");
		}
	}
	
	if(!b){
		luabind::functor<LPCSTR>			fl;
		R_ASSERT							(ai().script_engine().functor<LPCSTR>(cb_name,fl));
		fl									();
	}


}

bool CActorCondition::ApplyInfluence(const SMedicineInfluenceValues& V, const shared_str& sect)
{
	if (m_curr_medicine_influence.InProcess())
		return false;

	if (m_object->Local() && m_object == Level().CurrentViewEntity())
	{
		if (pSettings->line_exist(sect, "use_sound"))
		{
			if (m_use_sound._feedback())
				m_use_sound.stop();

			shared_str snd_name = pSettings->r_string(sect, "use_sound");
			m_use_sound.create(snd_name.c_str(), st_Effect, sg_SourceType);
			m_use_sound.play(NULL, sm_2D);
		}
	}

	if (V.fTimeTotal < 0.0f)
		return inherited::ApplyInfluence(V, sect);

	m_curr_medicine_influence = V;
	m_curr_medicine_influence.fTimeCurrent = m_curr_medicine_influence.fTimeTotal;
	return true;
}
bool CActorCondition::ApplyBooster(const SBooster& B, const shared_str& sect)
{
	if (!fis_zero(B.fBoostValue))
	{
		if (m_object->Local() && m_object == Level().CurrentViewEntity())
		{
			if (pSettings->line_exist(sect, "use_sound"))
			{
				if (m_use_sound._feedback())
					m_use_sound.stop();

				shared_str snd_name = pSettings->r_string(sect, "use_sound");
				m_use_sound.create(snd_name.c_str(), st_Effect, sg_SourceType);
				m_use_sound.play(NULL, sm_2D);
			}
		}

		BOOSTER_MAP::iterator it = m_booster_influences.find(B.m_type);
		if (it != m_booster_influences.end())
			DisableBoostParameters((*it).second);

		//in case the new effect is weaker, we shouldn't apply it
		m_booster_influences[B.m_type] = B;
		BoostParameters(B);
	}
	return true;
}

float CActorCondition::GetBoosterInfluence(EBoostParams type)
{
	BOOSTER_MAP::iterator it = m_booster_influences.find(type);
	if (it != m_booster_influences.end())
		return it->second.fBoostValue;
	else return 0.f;
}
