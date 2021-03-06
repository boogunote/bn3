#include "stdafx.h"
#include "BooTextFieldUI.h"
#include "BooFileViewNodeUI.h"
#include "BooFileViewUI.h"

extern int g_nTextHeight;

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
	this->SetEventMask(ENM_REQUESTRESIZE);
	this->SetAutoURLDetect(true);
	this->SetText(L".");
	this->SetText(L"");
}

LRESULT BooTextFieldUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//提高处理效率，看看是不是发生在本控件上的鼠标消息，或者本控件是不是焦点控件。
	bool bShouldHandle = false;
	if (WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST)
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if (!::PtInRect(&m_rcItem, pt))
		{
			return 0;
		}
	}
	else if (!this->IsFocused())
 	{
		return 0;
 	}


	if (WM_KEYDOWN == uMsg)
	{
// 		switch (wParam)
// 		{
// 		default:
// 			;
// 		}
	}
	else if (WM_LBUTTONDOWN == uMsg)
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

		if ((GetKeyState(VK_CONTROL) & 0x8000))
		{
			m_pManager->SendNotify(this, _T("selectnode")); //注意，这里select的意思不是焦点所在，而是按住ctrl选择
			bHandled = true;
		}
		else if ((GetKeyState(VK_SHIFT) & 0x8000))
		{
			m_pManager->SendNotify(this, _T("selectmultinode"));
			bHandled = true;
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
			(m_szRequest.cx != preqsz->rc.right ||	m_szRequest.cy != preqsz->rc.bottom + 2)) //注意，这里+4是为了留出上下2个像素的空白，不然上下就会显得太挤
		{
			m_szRequest.cx = preqsz->rc.right;
			m_szRequest.cy = preqsz->rc.bottom + 2;//注意，这里+4是为了留出上下2个像素的空白，不然上下就会显得太挤
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