// ActorCondition.h: ����� ��������� ������
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;



class CActor;
//class CUIActorSleepVideoPlayer;

class CActorCondition: public CEntityCondition {
private:
	typedef CEntityCondition inherited;
	enum {	eCriticalPowerReached			=(1<<0),
			eCriticalMaxPowerReached		=(1<<1),
			eCriticalBleedingSpeed			=(1<<2),
			eCriticalSatietyReached			=(1<<3),
			eCriticalRadiationReached		=(1<<4),
			eWeaponJammedReached			=(1<<5),
			ePhyHealthMinReached			=(1<<6),
			eCantWalkWeight					=(1<<7),
			};
	Flags16											m_condition_flags;
private:
	CActor*											m_object;
	void				UpdateTutorialThresholds	();
	void 				UpdateSatiety				();
public:
						CActorCondition				(CActor *object);
	virtual				~CActorCondition			(void);

	virtual void		LoadCondition				(LPCSTR section);
	virtual void		reinit						();

	virtual CWound*		ConditionHit				(SHit* pHDS);
	virtual void		UpdateCondition				();
			void		UpdateBoosters();

	virtual void 		ChangeAlcohol				(float value);
	virtual void 		ChangeSatiety				(float value);

	void 				BoostParameters(const SBooster& B);
	void 				DisableBoostParameters(const SBooster& B);
	IC void				BoostMaxWeight(const float value);

	IC void				BoostHpRestore(const float value);
	IC void				BoostPsyRestore(const float value);
	IC void				BoostSatietyRestore(const float value);
	IC void				BoostPowerRestore(const float value);
	IC void				BoostRadiationRestore(const float value);
	IC void				BoostAlcoholRestore(const float value);
	IC void				BoostBleedingRestore(const float value);

	IC void				BoostBurnImmunity(const float value);
	IC void				BoostShockImmunity(const float value);
	IC void				BoostRadiationImmunity(const float value);
	IC void				BoostTelepaticImmunity(const float value);
	IC void				BoostChemicalBurnImmunity(const float value);
	IC void				BoostExplImmunity(const float value);
	IC void				BoostStrikeImmunity(const float value);
	IC void				BoostFireWoundImmunity(const float value);
	IC void				BoostWoundImmunity(const float value);

	IC void				BoostBurnProtection(const float value);
	IC void				BoostShockProtection(const float value);
	IC void				BoostRadiationProtection(const float value);
	IC void				BoostTelepaticProtection(const float value);
	IC void				BoostChemicalBurnProtection(const float value);
	IC void				BoostExplProtection(const float value);
	IC void				BoostStrikeProtection(const float value);
	IC void				BoostFireWoundProtection(const float value);
	IC void				BoostWoundProtection(const float value);
	BOOSTER_MAP			GetCurBoosterInfluences() { return m_booster_influences; };
	float				GetBoosterInfluence(EBoostParams type);

	// �������� ��� ������ ��� � ��������
	virtual	bool		IsLimping					() const;
	virtual bool		IsCantWalk					() const;
	virtual bool		IsCantWalkWeight			();
	virtual bool		IsCantSprint				() const;

			void		ConditionJump				(float weight);
			void		ConditionWalk				(float weight, bool accel, bool sprint);
			void		ConditionStand				(float weight);
			
			float	xr_stdcall	GetAlcohol			()	{return m_fAlcohol;}
			float	xr_stdcall	GetPsy				()	{return 1.0f-GetPsyHealth();}
			float				GetSatiety			()  {return m_fSatiety;}

public:
	IC		CActor		&object						() const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}

	virtual bool			ApplyInfluence(const SMedicineInfluenceValues& V, const shared_str& sect);
	virtual bool			ApplyBooster(const SBooster& B, const shared_str& sect);

	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

protected:
	SMedicineInfluenceValues						m_curr_medicine_influence;
	float m_fAlcohol;
	float m_fV_Alcohol;
//--
	float m_fSatiety;
	float m_fV_Satiety;
	float m_fV_SatietyPower;
	float m_fV_SatietyHealth;
	float m_fSatietyCritical;
//--
	float m_fPowerLeakSpeed;

	float m_fJumpPower;
	float m_fStandPower;
	float m_fWalkPower;
	float m_fJumpWeightPower;
	float m_fWalkWeightPower;
	float m_fOverweightWalkK;
	float m_fOverweightJumpK;
	float m_fAccelK;
	float m_fSprintK;
	
	float	m_MaxWalkWeight;

	mutable bool m_bLimping;
	mutable bool m_bCantWalk;
	mutable bool m_bCantSprint;

	//����� ���� � �������� ������ �������� ����� �������� �������
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fCantWalkPowerBegin;
	float m_fCantWalkPowerEnd;

	float m_fCantSprintPowerBegin;
	float m_fCantSprintPowerEnd;

	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;

	ref_sound m_use_sound;
};
