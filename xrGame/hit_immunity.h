// hit_immunity.h: ����� ��� ��� ��������, ������� ������������
//				   ������������ ���������� ��� ������ ����� �����
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "hit_immunity_space.h"

class CHitImmunity
{
public:
	CHitImmunity();
	virtual ~CHitImmunity();

	virtual void LoadImmunities (LPCSTR section,CInifile* ini);
	
	//������������ ��� ���������
	//virtual void AddImmunities(LPCSTR section,CInifile* ini);

	//
	float		GetHitImmunity(ALife::EHitType hit_type) const { return m_HitTypeK[hit_type]; }
	virtual float AffectHit		(float power, ALife::EHitType hit_type) const { return power * GetHitImmunity(hit_type); }

protected:
	//������������ �� ������� ����������� ���
	//��� ��������������� ���� �����������
	//(��� �������� �������� � ����������� ��������)
	HitImmunity::HitTypeSVec m_HitTypeK;
};