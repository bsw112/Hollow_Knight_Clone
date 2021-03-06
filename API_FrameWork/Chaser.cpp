#include "stdafx.h"
#include "Chaser.h"
#include "BmpMgr.h"
#include "ScrollMgr.h"
#include "TileMgr.h"
#include "MyTime.h"

CChaser::CChaser()
{
	ZeroMemory(m_pFrameKey, sizeof(TCHAR) * DIR_LEN);
}


CChaser::~CChaser()
{
}

void CChaser::Initialize()
{

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Monster/Husk/move.bmp", L"husk_move");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Monster/Husk/idle.bmp", L"husk_idle");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Monster/Husk/attack.bmp", L"husk_attack");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/Monster/Husk/dead.bmp", L"husk_dead");

	memcpy(m_pFrameKey, L"husk_idle", DIR_LEN);

	m_tInfo.iCX = 75;
	m_tInfo.iCY = 75;
	m_tImgInfo.iCX = 192;
	m_tImgInfo.iCY = 192;

	m_tStat = STAT(50);

	m_fDir = 1.f;

	//스폰된 장소가 정찰의 중심점
	m_PartolSpot.x = (LONG)m_tInfo.fX;
	m_PartolSpot.y = (LONG)m_tInfo.fY;

	m_fRadius = 500;
	m_fPatrol = 100;
	m_fSpeed = 1.f;
}

int CChaser::Update()
{

	if (m_bDead)
		return OBJ_DEAD;

#pragma region 넉백

	//델타타임 구하기
	m_fDeltaTime = CMyTime::Get_Instance()->Get_DeltaTime();
	//프레임사이의 간격이 너무 크면 안됨.
	if (m_fDeltaTime > 0.15f)
		m_fDeltaTime = 0.15f;


	if (m_dwForceTimer + m_fForceTime * 1000 > GetTickCount())
	{
		m_tInfo.fX += m_velocity.fX * m_fDeltaTime;
		m_tInfo.fY += m_velocity.fY * m_fDeltaTime;
	}
	else
	{
		m_fForceTime = 0.f;
		m_velocity.fX = 0;
		m_velocity.fY = 0;
	}
#pragma endregion



	//점프일때는 따로 콜리전 체크해줌
	CTileMgr::COLLISION collision = CTileMgr::END;
	CTileMgr::Get_Instance()->Collision_Ex(this, collision);

	//중력
	m_tInfo.fY += m_Gravity.fY;


	if (m_eCurState != STATE::DEAD)
	{
		float target_fX = m_pTarget->Get_INFO().fX;

		//타깃과의 거리측정
		m_distToTarget = (Vector2(m_pTarget->Get_INFO().fX, m_pTarget->Get_INFO().fY) - Vector2(m_tInfo.fX, m_tInfo.fY)).magnitude();

		//타겟이 인식범위안에 들어오면
		if (m_distToTarget < m_fRadius)
		{
			m_eCurState = STATE::RUN;
			//추격상태가 된다.
			Chase_Target();
		}
		else
		{
			m_eCurState = STATE::IDLE;
			//아니면 정찰
			Patrol();
		}
	}
	else //죽었으면
	{
		//일정시간후 파괴
		if (m_dwDeadTimer + m_fDeadWait * 1000 < GetTickCount())
			m_bDead = true;
	}




	Move_Frame();
	Scene_Change();
	return OBJ_NOEVENT;
}

void CChaser::Late_Update()
{
}



void CChaser::Release()
{
}

void CChaser::Patrol()
{


	//순찰범위를 벗어나면
	if (m_tInfo.fX < m_PartolSpot.x - m_fPatrol
		|| m_tInfo.fX > m_PartolSpot.x + m_fPatrol)
	{

		m_eCurState = STATE::WALK;

		Vector2 dirToPatrol = Vector2((float)m_PartolSpot.x, (float)m_PartolSpot.y) - Vector2(m_tInfo.fX, m_tInfo.fY);
		//순찰 장소와의 거리측정
		float distToPatrol = dirToPatrol.magnitude();

		dirToPatrol = dirToPatrol.Nomalize();

		//도착할때까지
		if (distToPatrol > 2)
		{
			//순찰장소로 간다.
			m_tInfo.fX += dirToPatrol.fX * m_fSpeed;

			dirToPatrol.fX < 0 ? m_fDir = -1 : m_fDir = 1;

		}

	}
}


void CChaser::Chase_Target()
{


	//1.5초 간격으로 추격
	static DWORD timer = GetTickCount();
	//1.5초에 한번씩 뒤바뀌는 락
	static bool lock = false;
	if (timer + 1500 < GetTickCount())
	{
		timer = GetTickCount();
		lock = !lock;
		m_pTarget->Get_INFO().fX - m_tInfo.fX < 0 ? m_fDir = -1 : m_fDir = 1;
	}

	//1.5초 동안
	if (lock)
	{
		//왼쪽으로 감
		m_tInfo.fX += (m_fSpeed + 4) * m_fDir;

	}
	else
		m_eCurState = STATE::IDLE;


}

void CChaser::Scene_Change()
{

	//거리가 너무 가까워지면 정신없이 바뀜
	if (m_distToTarget > 1.f)
	{
		//방향에 따라 스프라이트 시트 바꾸기.
		if (m_fDir < 0)
			m_eFront = FRONT::LEFT;
		else
			m_eFront = FRONT::RIGHT;
	}


	if (m_eCurState != m_ePrvState)
	{
		switch (m_eCurState)
		{
		case CChaser::IDLE:
		{
			memcpy(m_pFrameKey, L"husk_idle", DIR_LEN);
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.iFrameScene = m_eFront;
			m_tFrame.dwFrameTime = GetTickCount();
			m_tFrame.dwFrameSpeed = 200;
			m_tFrame.bLoop = true;
			break;
		}
		case CChaser::WALK:
		{
			memcpy(m_pFrameKey, L"husk_move", DIR_LEN);
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.iFrameScene = m_eFront;
			m_tFrame.dwFrameTime = GetTickCount();
			m_tFrame.dwFrameSpeed = 200;
			m_tFrame.bLoop = true;
			break;
		}
		case CChaser::RUN:
		{
			memcpy(m_pFrameKey, L"husk_attack", DIR_LEN);
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 10;
			m_tFrame.iFrameScene = m_eFront;
			m_tFrame.dwFrameTime = GetTickCount();
			m_tFrame.dwFrameSpeed = 100;
			m_tFrame.bLoop = true;
			break;
		}
		case CChaser::DEAD:
		{
			memcpy(m_pFrameKey, L"husk_dead", DIR_LEN);
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 9;
			m_tFrame.iFrameScene = m_eFront;
			m_tFrame.dwFrameTime = GetTickCount();
			m_tFrame.dwFrameSpeed = 200;
			m_tFrame.bLoop = false;
			break;
		}
		case CChaser::END:
			break;
		default:
			break;
		}

		m_ePrvState = m_eCurState;
	}
}

void CChaser::OnDead()
{
	m_eCurState = STATE::DEAD;
	memcpy(m_pFrameKey, L"husk_dead", DIR_LEN);
	m_dwDeadTimer = GetTickCount();
}

void CChaser::OnTakeDamage()
{
}


