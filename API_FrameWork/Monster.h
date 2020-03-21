#pragma once

#ifndef __MONSTER_H__
#define __MONSTER_H__


#include "Obj.h"
class CMonster : public CObj
{
public:
	CMonster();
	virtual ~CMonster();

public:
	virtual void Initialize()  = 0;
	virtual int Update()  = 0;
	virtual void Late_Update()  = 0;
	virtual void Render(HDC _DC)  = 0;
	virtual void Release() = 0 ;

protected:
	virtual void Patrol() = 0;
	virtual void Chase_Target() = 0;
	virtual void Scene_Change() = 0;
	
protected:
	//�νĹ����� �÷��̾ ���Դ°�?
	bool IsAlert();
	void Take_Damage(float _fDamage) { m_tStat.m_fHp -= _fDamage; if (m_tStat.m_fHp < 0) m_tStat.m_fHp = 0; }

protected:
	STAT m_tStat;
	//�νĹ���
	float m_fRadius;
	//��������
	float m_fPatrol;
	//���� ������
	POINT m_PartolSpot;
};


#endif // !__MONSTER_H__