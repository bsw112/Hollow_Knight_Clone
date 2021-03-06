#include "stdafx.h"
#include "Obj.h"


CObj::CObj()
	: m_fSpeed(0.f), m_bDead(false), m_fAngle(0.f), m_eTag(OBJTAG::END),
	m_bCollided(false), m_iCurrLoopCnt(0)
{
	ZeroMemory(&m_tInfo, sizeof(m_tInfo));
	ZeroMemory(&m_tRect, sizeof(m_tRect));
	ZeroMemory(&m_tImgInfo, sizeof(m_tInfo));
	ZeroMemory(&m_tImgRect, sizeof(m_tRect));
	ZeroMemory(&m_tFrame, sizeof(m_tFrame));
	ZeroMemory(m_pFrameKey, sizeof(m_pFrameKey));
}


CObj::~CObj()
{
}


void CObj::Move_Frame()
{
	if (m_tFrame.dwFrameTime + m_tFrame.dwFrameSpeed < GetTickCount())
	{

		if (m_tFrame.iFrameStart < m_tFrame.iFrameEnd)
		{
			++m_tFrame.iFrameStart;
			m_tFrame.bEnd = false;
		}
		else
		{
			if (m_iCurrLoopCnt >= m_tFrame.iLoopCnt)
			{
				m_tFrame.bEnd = true;
				m_iCurrLoopCnt = 0;
			}
			else
				++m_iCurrLoopCnt;

			if (m_tFrame.bLoop)
			{
				m_tFrame.iFrameStart = 0;
			}
			else
			{
				m_tFrame.iFrameStart = m_tFrame.iFrameEnd;
			}
		}

		m_tFrame.dwFrameTime = GetTickCount();
	}

}



void CObj::OnCollisionEnter(CObj * _pOther, float _fX, float _fY)
{
}

void CObj::OnCollisionStay(CObj * _pOther, float _fX, float _fY)
{

}

void CObj::OnCollisionExit(CObj * _pOther, float _fX, float _fY)
{
}



void CObj::Update_Rect()
{

	m_tRect.left = LONG(m_tInfo.fX - (m_tInfo.iCX >> 1));
	m_tRect.top = LONG(m_tInfo.fY - (m_tInfo.iCY >> 1));
	m_tRect.right = LONG(m_tInfo.fX + (m_tInfo.iCX >> 1));
	m_tRect.bottom = LONG(m_tInfo.fY + (m_tInfo.iCY >> 1));

	m_tImgInfo.fX = m_tInfo.fX;
	m_tImgInfo.fY = m_tInfo.fY;

	m_tImgRect.left = LONG(m_tImgInfo.fX - (m_tImgInfo.iCX >> 1));
	m_tImgRect.top = LONG(m_tImgInfo.fY - (m_tImgInfo.iCY >> 1));
	m_tImgRect.right = LONG(m_tImgInfo.fX + (m_tImgInfo.iCX >> 1));
	m_tImgRect.bottom = LONG(m_tImgInfo.fY + (m_tImgInfo.iCY >> 1));


}

void CObj::Set_Pos(float _x, float _y)
{
	m_tInfo.fX = _x;
	m_tInfo.fY = _y;
}
