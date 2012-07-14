#include "stdafx.h"
#include "BooTextFieldUI.h"
#include "BooFileViewNodeUI.h"
#include "BooFileViewUI.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooTextFieldUI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooTextFieldUI::BooTextFieldUI()
{
}

LPCTSTR BooTextFieldUI::GetClass() const
{
	return _T("BooTextFieldUI");
}

LPVOID BooTextFieldUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooTextField")) == 0 ) return static_cast<BooTextFieldUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void BooTextFieldUI::DoInit()
{
	__super::DoInit();
	this->SetEventMask(ENM_REQUESTRESIZE|ENM_LINK);
	this->SetText(L".");
	this->SetText(L"");
}

LRESULT BooTextFieldUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//提高处理效率
	if (!this->IsFocused())
	{
		return 0;
	}

	if (WM_KEYDOWN == uMsg)
	{
		switch (wParam)
		{
		case VK_RETURN:
			{
				if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
				{
					if(GetManager()->GetFocus() == this) {
						m_pManager->SendNotify(this, _T("createnode"), CREATENODE_PREVIOUS);
						bHandled = TRUE;
					}
				}
				else if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					if(GetManager()->GetFocus() == this) {
						m_pManager->SendNotify(this, _T("createnode"), CREATENODE_NEXT);
						bHandled = TRUE;
					}
				}
				else if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					if(GetManager()->GetFocus() == this) {
						m_pManager->SendNotify(this, _T("createnode"), CREATENODE_CHILD);
						bHandled = TRUE;
					}
				}
			}
		default:
			;
		}
	}
	if (bHandled)
	{
		return 0;
	}
	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void BooTextFieldUI::OnTxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_REQUESTRESIZE )
	{
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		if (0 != preqsz->rc.bottom &&
			(m_szRequest.cx != preqsz->rc.right ||	m_szRequest.cy != preqsz->rc.bottom + 4)) //注意，这里+4是为了留出上下2个像素的空白，不然上下就会显得太挤
		{
			m_szRequest.cx = preqsz->rc.right;
			m_szRequest.cy = preqsz->rc.bottom + 4;//注意，这里+4是为了留出上下2个像素的空白，不然上下就会显得太挤
			WCHAR szHeight[BUF_128B];
			_itow_s(m_szRequest.cy, szHeight, BUF_128B, 10);
			this->SetAttribute(L"height", szHeight);
			m_pManager->SendNotify(this, _T("heightchanged"), m_szRequest.cy);
		}
	}
}

void BooTextFieldUI::DoEvent(TEventUI& event)
{
	// 	if( event.Type == UIEVENT_SETFOCUS )
	// 	{
	// 		m_pManager->SendNotify(this, _T("setnodefocus"));
	// 	}
	__super::DoEvent(event);
}