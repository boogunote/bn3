#include "stdafx.h"
#include "BooTextFieldUI.h"
#include "BooFileViewNodeUI.h"
#include "BooFileViewUI.h"

extern int g_nTextHeight;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooFileViewNode
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooFileViewNodeUI::BooFileViewNodeUI() : m_nIndent(0), m_bExpand(true), m_bHasChild(false), m_bSelected(false), m_bDisable(false)
{
	m_indent = new CControlUI;
	m_indent->ApplyAttributeList(_T("float=\"false\" bordersize=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\""));
	this->Add(m_indent);

	m_button = new CButtonUI;
	int nButtonHeight = 15; //最好是奇数，这样画连接线的时候能够对齐。
	int nPadding = (g_nTextHeight-nButtonHeight)/2;
	CStdString strAttr;
	strAttr.Format(_T("pos=\"0,0,%d,%d\" padding=\"0, %d,0,%d\""),nButtonHeight,nButtonHeight,nPadding,nPadding);
	m_button->ApplyAttributeList(strAttr);
	this->Add(m_button);
	m_button->OnNotify += MakeDelegate(this, &BooFileViewNodeUI::OnButtonNotify);

	CControlUI* pSpaceControl = new CControlUI;
	pSpaceControl->ApplyAttributeList(_T("width=\"3\""));
	this->Add(pSpaceControl);

	m_text = new BooTextFieldUI;
	m_text->ApplyAttributeList(_T("width=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FFEEEEEE\" bordersize=\"1\" focusbordercolor=\"#FF00A2E8\" inset=\"2,2,2,2\""));
	this->Add(m_text);
	m_text->OnNotify += MakeDelegate(this, &BooFileViewNodeUI::OnTextFeildNotify);
}

LPCTSTR BooFileViewNodeUI::GetClass() const
{
	return _T("BooFileViewNode");
}

LPVOID BooFileViewNodeUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooFileViewNode")) == 0 ) return static_cast<BooFileViewNodeUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void BooFileViewNodeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	__super::SetAttribute(pstrName, pstrValue);
	if( _tcscmp(pstrName, _T("indent")) == 0 )
	{
		m_nIndent = _ttoi(pstrValue);
		m_indent->SetFixedWidth(m_nIndent*30+1); //最小必须是1，不能是0,0就是自动撑开
	}
}

bool BooFileViewNodeUI::OnButtonNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if (pMsg->sType == _T("click"))
	{
		m_text->SetFocus();
		m_pManager->SendNotify(this, _T("statebuttonclick"), pMsg->wParam, pMsg->lParam);
	}
	return true;
}

bool BooFileViewNodeUI::OnTextFeildNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if( pMsg->sType == _T("heightchanged") )
	{
		WCHAR szHeight[BUF_128B];
		_itow_s(pMsg->wParam, szHeight, BUF_128B, 10);
		this->SetAttribute(L"height", szHeight);
	}
	else if (pMsg->sType == _T("createnode")) //接力转发createnode消息
	{
		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	}
	else if (pMsg->sType == _T("selectnode"))
	{
		m_bSelected = !m_bSelected;
		SetSelect(m_bSelected);

	}
	else if (pMsg->sType == _T("cleanselect"))
	{
		m_pManager->SendNotify(this, _T("cleanselect"));
	}
	else if (pMsg->sType == _T("movefocus"))
	{
		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	}
	// 	else if( pMsg->sType == _T("setnodefocus")) //接力转发setnodefocus消息
	// 	{
	// 		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	// 	}
	return true;
}

void BooFileViewNodeUI::DoInit()
{
	m_text->SetName(this->GetName());
}

void BooFileViewNodeUI::SetFocus()
{
	m_text->SetFocus();
}

void BooFileViewNodeUI::UpdateStateButton()
{
	if (m_bHasChild)
	{
		if (m_bExpand)
		{
			m_button->ApplyAttributeList(_T("normalimage=\"file='statenode.png' source='0,15,15,30'\" hotimage=\"file='statenode.png' source='15,15,30,30'\" pushedimage=\"file='statenode.png' source='30,15,45,30'\""));
		}
		else
		{
			m_button->ApplyAttributeList(_T("normalimage=\"file='statenode.png' source='0,0,15,15'\" hotimage=\"file='statenode.png' source='15,0,30,15'\" pushedimage=\"file='statenode.png' source='30,0,45,15'\""));
		}
	}
	else
	{
		m_button->ApplyAttributeList(_T("normalimage=\"file='statenode.png' source='0,30,15,45'\" hotimage=\"file='statenode.png' source='15,30,30,45'\" pushedimage=\"file='statenode.png' source='30,30,45,45'\""));
	}
}

void BooFileViewNodeUI::SetSelect(bool bSelect)
{
	m_bSelected = bSelect;
	if (m_bSelected)
	{
		m_text->SetAttribute(_T("bkcolor"),_T("#FF99D9EA"));
	}
	else
	{
		m_text->SetAttribute(_T("bkcolor"),_T("#FFFFFFFF"));
	}
}