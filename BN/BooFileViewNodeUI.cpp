#include "stdafx.h"
#include "BooTextFieldUI.h"
#include "BooFileViewNodeUI.h"
#include "BooFileViewUI.h"
#include "Helper.h"

extern int g_nTextHeight;
const int g_nIconWidth = 16;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooFileViewNode
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooFileViewNodeUI::BooFileViewNodeUI() : m_nIndent(0), m_bExpand(true), m_bHasChild(false), m_bSelected(false), m_bOneLine(false), m_nIconIndex(-1), m_bBold(false),  m_strContent(L"")
{
	m_indent = new CControlUI;
	m_indent->ApplyAttributeList(_T("float=\"false\" bordersize=\"0\" height=\"0\" bkcolor=\"#00FFFFFF\""));
	this->Add(m_indent);

	m_button = new CButtonUI;
	int nButtonHeight = 15; //最好是奇数，这样画连接线的时候能够对齐。
	int nPadding = (g_nTextHeight-nButtonHeight)/2;
	CStdString strAttr;
	strAttr.Format(_T("pos=\"0,0,%d,%d\" padding=\"0, %d,0,%d\" menu=\"true\""),nButtonHeight,nButtonHeight,nPadding,nPadding);
	m_button->ApplyAttributeList(strAttr);
	this->Add(m_button);
	m_button->OnNotify += MakeDelegate(this, &BooFileViewNodeUI::OnButtonNotify);

	m_icon = new CControlUI;
	nPadding = (g_nTextHeight-g_nIconWidth)/2;
	strAttr.Format(_T("width=\"3\" padding=\"2, %d,2,%d\""), nPadding,nPadding);
	m_icon->ApplyAttributeList(strAttr);
	SetIconIndex(m_nIconIndex);

	
	this->Add(m_icon);

	m_text = new BooTextFieldUI;
	m_text->ApplyAttributeList(_T("width=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FFEEEEEE\" bordersize=\"1\" focusbordercolor=\"#FF00A2E8\" inset=\"2,0,0,0\" padding=\"0,1,0,1\""));
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
	else if (pMsg->sType == _T("menu"))
	{
		m_text->SetFocus();
		SetOneLine(!m_bOneLine);

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
	else if (pMsg->sType == _T("selectnode"))
	{
		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	}
	else if (pMsg->sType == _T("selectmultinode"))
	{
		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	}
	else if( pMsg->sType == _T("setfocus")) //接力转发setnodefocus消息
	{
		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
	}

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
		SetAttribute(_T("bkcolor"),_T("#FF99D9EA"));
	}
	else
	{
		SetAttribute(_T("bkcolor"),_T("#FFFFFFFF"));
	}
}

void BooFileViewNodeUI::SetIconIndex(int nIconIndex)
{
	m_nIconIndex = nIconIndex;
	CStdString strAttr;
	if (m_nIconIndex >= 0)
	{
		int nLeft = g_nIconWidth*m_nIconIndex;
		int nRight = g_nIconWidth*(m_nIconIndex+1);
		strAttr.Format(_T("file='bn_icons.bmp' dest='0,0,16,16' source='%d,0,%d, 16' mask='#FFC0C0C0'"), nLeft, nRight);
		m_icon->SetBkImage(strAttr);
		m_icon->SetFixedWidth(g_nIconWidth);
	}
	else
	{
		m_icon->SetBkImage(L"");
		m_icon->SetFixedWidth(1); //必须要是1,0的话就要参加布局
	}
}

void BooFileViewNodeUI::SetContent(LPCTSTR lpcstrContent)
{
	if (m_bOneLine)
	{
		m_strContent = lpcstrContent;
		m_text->SetText(m_strContent.Mid(0, 10)+_T("..."));
	}
	else
	{
		m_text->SetText(lpcstrContent);
	}
}

void BooFileViewNodeUI::GetContent(CStdString& strContent)
{
	if (m_bOneLine)
	{
		strContent = m_strContent;
	}
	else
	{
		strContent = m_text->GetText();
	}
}

void BooFileViewNodeUI::SetOneLine( bool bOneLine, bool bIsInit)
{
	m_bOneLine = bOneLine;
	if (m_bOneLine)
	{
		//如果是在创建节点的时候使用，则不要进行这个同步
		if (!bIsInit)
		{
			m_strContent = m_text->GetText();
		}
		m_text->SetText(m_strContent.Mid(0, 10)+L"...");
		m_text->SetReadOnly(true);
		m_text->SetMouseEnabled(false);
		m_bOneLine = true;
	}
	else
	{
		m_text->SetText(m_strContent);
		m_strContent.Empty();
		m_text->SetReadOnly(false);
		m_text->SetMouseEnabled(true);
		m_bOneLine = false;
	}
}

void BooFileViewNodeUI::SetHasChildren( bool bHasChildren )
{
	m_bHasChild = bHasChildren;
	UpdateStateButton();
}

void BooFileViewNodeUI::SetBold( bool bBold )
{
	m_bBold = bBold;
	if (m_bBold)
	{
		m_text->SetFont(2);
	}
	else
	{
		m_text->SetFont(3);
	}
}

void BooFileViewNodeUI::SetTextColorV7( LPCTSTR lpcstrColor )
{
	CStdString strColor = lpcstrColor;
	strColor = strColor.Mid(0,6);
	strColor = CStdString(_T("#ff")) + strColor;
	m_text->SetAttribute(_T("textcolor"), strColor);
}

void BooFileViewNodeUI::SetBkColorV7( LPCTSTR lpcstrColor )
{
	CStdString strColor = lpcstrColor;
	strColor = strColor.Mid(0,6);
	strColor = CStdString(_T("#ff")) + strColor;
	m_text->SetAttribute(_T("bkcolor"), strColor);
}

void BooFileViewNodeUI::SetTextColor( LPCTSTR lpcstrColor )
{
	m_text->SetAttribute(_T("textcolor"), lpcstrColor);
}

void BooFileViewNodeUI::SetBkColor( LPCTSTR lpcstrColor )
{
	m_text->SetAttribute(_T("bkcolor"), lpcstrColor);
}

DWORD BooFileViewNodeUI::GetTextFieldTextColor()
{
	return m_text->GetTextColor();
}

DWORD BooFileViewNodeUI::GetTextFieldBkColor()
{
	return m_text->GetBkColor();
}