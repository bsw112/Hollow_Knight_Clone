#pragma once
#include "Monster.h"
class CChaser : public CMonster
{
public:
	CChaser();
	virtual ~CChaser();

public:
	enum STATE { IDLE, WALK, RUN, HIT, DEAD, END };


	// CMonster을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _DC) override;
	virtual void Release() override;

public:
	virtual void Patrol() override;
	virtual void Chase_Target() override;
	virtual void Scene_Change() override;

private:
	STATE			m_eCurState;
	STATE			m_ePrvState;


};

