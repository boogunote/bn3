#include "stdafx.h"
#include "BooFileView.h"


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
	if( _tcscmp(pstrName, _T("BooTextField")) == 0 ) return static_cast<CRichEditUI*>(this);
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
	if (WM_KEYDOWN == uMsg)
	{
		switch (wParam)
		{
		case VK_RETURN:
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					m_pManager->SendNotify(this, _T("createnode"));
				}
			}
		default:
			;
		}
	}
	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void BooTextFieldUI::OnTxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_REQUESTRESIZE ) {
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		if (m_szRequest.cx != preqsz->rc.right ||
			m_szRequest.cy != preqsz->rc.bottom)
		{
			m_szRequest.cx = preqsz->rc.right;
			m_szRequest.cy = preqsz->rc.bottom;
			WCHAR szHeight[BUF_128B];
			_itow_s(m_szRequest.cy, szHeight, BUF_128B, 10);
			this->SetAttribute(L"height", szHeight);
			m_pManager->SendNotify(this, _T("heightchanged"), m_szRequest.cy);
			//this->GetParent()->SetAttribute(L"height", szHeight);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooFileViewNode
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooFileViewNode::BooFileViewNode() : m_nIndent(0)
{
	m_indent = new CControlUI;
	m_indent->ApplyAttributeList(_T("float=\"false\" bordersize=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\""));
	this->Add(m_indent);

	m_text = new BooTextFieldUI;
	m_text->ApplyAttributeList(_T("width=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FF0000FF\" bordersize=\"1\""));
	m_text->OnNotify += MakeDelegate(this, &BooFileViewNode::OnTextFeildNotify);
	this->Add(m_text);
}

LPCTSTR BooFileViewNode::GetClass() const
{
	return _T("BooFileViewNode");
}

LPVOID BooFileViewNode::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooFileViewNode")) == 0 ) return static_cast<BooFileViewNode*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void BooFileViewNode::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	__super::SetAttribute(pstrName, pstrValue);
	if( _tcscmp(pstrName, _T("indent")) == 0 )
	{
		m_nIndent = _ttoi(pstrValue);
		m_indent->SetFixedWidth(m_nIndent*30+1); //最小必须是1，不能是0,0就是自动撑开
	}
}

bool BooFileViewNode::OnTextFeildNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if( pMsg->sType == _T("heightchanged") ) {
		WCHAR szHeight[BUF_128B];
		_itow_s(pMsg->wParam, szHeight, BUF_128B, 10);
		this->SetAttribute(L"height", szHeight);
	}
	else if (pMsg->sType == _T("createnode")) //接力转发createnode消息
	{
		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooFileViewUI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooFileViewUI::BooFileViewUI()
{
}

LPCTSTR BooFileViewUI::GetClass() const
{
	return _T("BooFileViewUI");
}

LPVOID BooFileViewUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooFileViewUI")) == 0 ) return static_cast<BooFileViewUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void BooFileViewUI::DoInit()
{
	__super::DoInit();
	for (int i=0; i<m_items.GetSize(); i++)
	{
		static_cast<CControlUI*>(m_items[i])->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);;
	}
}

bool BooFileViewUI::OnNodeNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if( pMsg->sType == _T("createnode") )
	{
		int a = 0;
	}
	return true;
}

void BooFileViewUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
}