#pragma once

class CWound;
class NET_Packet;
class CEntityAlive;
class CLevel;

#include "hit_immunity.h"
#include "Hit.h"
#include "Level.h"

enum EBoostParams {
	eBoostHpRestore = 0,
	eBoostPsyHealthRestore,
	eBoostRadiationRestore,
	eBoostSatietyRestore,
	eBoostPowerRestore,
	eBoostBleedingRestore,
	eBoostAlcoholRestore,

	eBoostMaxWeight,

	eBoostBurnProtection,
	eBoostShockProtection,
	eBoostRadiationProtection,
	eBoostTelepaticProtection,
	eBoostChemicalBurnProtection,
	eBoostExplProtection,
	eBoostStrikeProtection,
	eBoostFireWoundProtection,
	eBoostWoundProtection,

	eBoostBurnImmunity,
	eBoostShockImmunity,
	eBoostRadiationImmunity,
	eBoostTelepaticImmunity,
	eBoostChemicalBurnImmunity,
	eBoostExplImmunity,
	eBoostStrikeImmunity,
	eBoostFireWoundImmunity,
	eBoostWoundImmunity,

	eBoostMaxCount,
};

static const LPCSTR ef_boosters_section_names[] =
{
	"boost_health_restore",
	"boost_psy_health_restore",
	"boost_radiation_restore",
	"boost_satiety_restore",
	"boost_power_restore",
	"boost_bleeding_restore",
	"boost_alcohol_restore",

	"boost_max_weight",

	"boost_burn_protection",
	"boost_shock_protection",
	"boost_radiation_protection",
	"boost_telepat_protection",
	"boost_chemburn_protection",
	"boost_explosion_protection",
	"boost_strike_protection",
	"boost_fire_wound_protection",
	"boost_wound_protection",

	"boost_burn_immunity",
	"boost_shock_immunity",
	"boost_radiation_immunity",
	"boost_telepat_immunity",
	"boost_chemburn_immunity",
	"boost_explosion_immunity",
	"boost_strike_immunity",
	"boost_fire_wound_immunity",
	"boost_wound_immunity"
};

struct SBooster {
	float fBoostTime;
	float fBoostValue;
	EBoostParams m_type;
	SBooster() :fBoostTime(-1.0f) {};
	void Load(const shared_str& sect, EBoostParams type);
};

struct SMedicineInfluenceValues {
	float fHealth;
	float fPsyHealth;
	float fPower;
	float fSatiety;
	float fRadiation;
	float fWoundsHeal;
	float fMaxPowerUp;
	float fAlcohol;
	float fTimeTotal;
	float fTimeCurrent;

	SMedicineInfluenceValues() :fTimeCurrent(-1.0f) {}
	bool InProcess() { return fTimeCurrent > 0.0f; }
	void Load(const shared_str& sect);
};

class CEntityConditionSimple
{
	float					m_fHealth;
	float					m_fHealthMax;
public:
							CEntityConditionSimple	();
	virtual					~CEntityConditionSimple	();

	IC float				GetHealth				() const			{return m_fHealth;}
	IC float 				GetMaxHealth			() const			{return m_fHealthMax;}
	IC float&				health					()					{return	m_fHealth;}
	IC float&				max_health				()					{return	m_fHealthMax;}
};

class CEntityCondition: public CEntityConditionSimple, public CHitImmunity
{
private:
	bool					m_use_limping_state;
	CEntityAlive			*m_object;

public:
							CEntityCondition		(CEntityAlive *object);
	virtual					~CEntityCondition		(void);

	virtual void			LoadCondition			(LPCSTR section);
	virtual void			remove_links			(const CObject *object);

	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

	virtual void			SetHealth				(float value);

	IC float				GetPower				() const			{return m_fPower;}	
	IC float				GetRadiation			() const			{return m_fRadiation;}
	IC float				GetPsyHealth			() const			{return m_fPsyHealth;}

	IC float 				GetEntityMorale			() const			{return m_fEntityMorale;}

	IC float 				GetHealthLost			() const			{return m_fHealthLost;}

	virtual bool 			IsLimping				() const;

	virtual void			ChangeSatiety			(float value)		{};
	void 					ChangeHealth			(float value);
	void 					ChangePower				(float value);
	void 					ChangeRadiation			(float value);
	void 					ChangePsyHealth			(float value);
	virtual void 			ChangeAlcohol			(float value){};

	IC void					MaxPower				()					{m_fPower = m_fPowerMax;};
	IC void					SetMaxPower				(float val)			{m_fPowerMax = val; clamp(m_fPowerMax,0.1f,1.0f);};
	IC float				GetMaxPower				() const			{return m_fPowerMax;};

	void 					ChangeBleeding			(float percent);

	void 					ChangeCircumspection	(float value);
	void 					ChangeEntityMorale		(float value);

	virtual CWound*			ConditionHit			(SHit* pHDS);
	//обновления состояния с течением времени
	virtual void			UpdateCondition			();
	void					UpdateWounds			();
	void					UpdateConditionTime		();
	IC void					SetConditionDeltaTime	(float DeltaTime) { m_fDeltaTime = DeltaTime; };

	
	//скорость потери крови из всех открытых ран 
	float					BleedingSpeed			();

	CObject*				GetWhoHitLastTime		() {return m_pWho;}
	u16						GetWhoHitLastTimeID		() {return m_iWhoID;}

	CWound*					AddWound				(float hit_power, ALife::EHitType hit_type, u16 element);

	IC void 				SetCanBeHarmedState		(bool CanBeHarmed) 			{m_bCanBeHarmed = CanBeHarmed;}
	IC bool					CanBeHarmed				() const					{return OnServer() && m_bCanBeHarmed;};

	typedef					xr_map<EBoostParams, SBooster> BOOSTER_MAP;
	virtual bool			ApplyInfluence(const SMedicineInfluenceValues& V, const shared_str& sect);
	virtual bool			ApplyBooster(const SBooster& B, const shared_str& sect);
	
	void					ClearWounds();
protected:
	void					UpdateHealth			();
	void					UpdatePower				();
	void					UpdateSatiety			(float k=1.0f);
	void					UpdateRadiation			(float k=1.0f);
	void					UpdatePsyHealth			(float k=1.0f);

	void					UpdateEntityMorale		();


	//изменение силы хита в зависимости от надетого костюма
	//(только для InventoryOwner)
	float					HitOutfitEffect			(SHit* hit);
	//изменение потери сил в зависимости от надетого костюма
	float					HitPowerEffect			(float power_loss);
	
	//для подсчета состояния открытых ран,
	//запоминается кость куда был нанесен хит
	//и скорость потери крови из раны
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR			m_WoundVector;
	//очистка массива ран
	

	//все величины от 0 до 1			
	float m_fPower;					//сила
	float m_fRadiation;				//доза радиактивного облучения
	float m_fPsyHealth;				//здоровье

	float m_fEntityMorale;			//мораль

	//максимальные величины
	//	float m_fSatietyMax;
	float m_fPowerMax;
	float m_fRadiationMax;
	float m_fPsyHealthMax;

	float m_fEntityMoraleMax;

	//величины изменения параметров на каждом обновлении
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaRadiation;
	float m_fDeltaPsyHealth;

	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	struct SConditionChangeV
	{
		float			m_fV_Radiation;
		float			m_fV_PsyHealth;
		float			m_fV_Circumspection;
		float			m_fV_EntityMorale;
		float			m_fV_RadiationHealth;
		float			m_fV_Bleeding;
		float			m_fV_WoundIncarnation;
		float			m_fV_HealthRestore;
		void			load(LPCSTR sect, LPCSTR prefix);
	};
	
	SConditionChangeV m_change_v;

	float				m_fMinWoundSize;
	bool				m_bIsBleeding;

	//части хита, затрачиваемые на уменьшение здоровья и силы
	float				m_fHealthHitPart;
	float				m_fPowerHitPart;

	//процентное уменьшение от бустеров (DR)
	float				m_fBoostBurnImmunity;
	float				m_fBoostShockImmunity;
	float				m_fBoostRadiationImmunity;
	float 				m_fBoostTelepaticImmunity;
	float 				m_fBoostChemicalBurnImmunity;
	float 				m_fBoostExplImmunity;
	float 				m_fBoostStrikeImmunity;
	float 				m_fBoostFireWoundImmunity;
	float 				m_fBoostWoundImmunity;

	//уменьшение изначального хита от бустеров (DT)
	float				m_fBoostBurnProtection;
	float				m_fBoostShockProtection;
	float				m_fBoostRadiationProtection;
	float 				m_fBoostTelepaticProtection;
	float 				m_fBoostChemicalBurnProtection;
	float 				m_fBoostExplProtection;
	float 				m_fBoostStrikeProtection;
	float 				m_fBoostFireWoundProtection;
	float 				m_fBoostWoundProtection;

	//потеря здоровья от последнего хита
	float				m_fHealthLost;


	//для отслеживания времени 
	u64					m_iLastTimeCalled;
	float				m_fDeltaTime;
	//кто нанес последний хит
	CObject*			m_pWho;
	u16					m_iWhoID;

	//для передачи параметров из DamageManager
	float				m_fHitBoneScale;
	float				m_fWoundBoneScale;

	float				m_limping_threshold;

	bool				m_bTimeValid;
	bool				m_bCanBeHarmed;
	BOOSTER_MAP			m_booster_influences;

public:
	virtual void					reinit				();
	
	IC const	float				fdelta_time			() const 	{return		(m_fDeltaTime);			}
	IC const	WOUND_VECTOR&		wounds				() const	{return		(m_WoundVector);		}
	IC float&						radiation			()			{return		(m_fRadiation);			}
	IC float&						hit_bone_scale		()			{return		(m_fHitBoneScale);		}
	IC float&						wound_bone_scale	()			{return		(m_fWoundBoneScale);	}
};
