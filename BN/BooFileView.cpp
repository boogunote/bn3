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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooFileViewNode
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooFileViewNode::BooFileViewNode() : m_nIndent(0), m_bExpand(true)
{
	m_indent = new CControlUI;
	m_indent->ApplyAttributeList(_T("float=\"false\" bordersize=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\""));
	this->Add(m_indent);

	m_button = new CButtonUI;
	m_button->ApplyAttributeList(_T("name=\"changeskinbtn\" bkcolor=\"#FF0000EE\" width=\"30\""));
	this->Add(m_button);
	m_button->OnNotify += MakeDelegate(this, &BooFileViewNode::OnButtonNotify);

	m_text = new BooTextFieldUI;
	m_text->ApplyAttributeList(_T("width=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FFEEEEEE\" bordersize=\"1\" focusbordercolor=\"#FF0000FF\" inset=\"2,2,2,2\""));
	this->Add(m_text);
	m_text->OnNotify += MakeDelegate(this, &BooFileViewNode::OnTextFeildNotify);
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

bool BooFileViewNode::OnButtonNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if (pMsg->sType == _T("click"))
	{
		m_pManager->SendNotify(this, _T("statebuttonclick"), pMsg->wParam, pMsg->lParam);
	}
	return true;
}

bool BooFileViewNode::OnTextFeildNotify(void* param)
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
// 	else if( pMsg->sType == _T("setnodefocus")) //接力转发setnodefocus消息
// 	{
// 		m_pManager->SendNotify(this, pMsg->sType, pMsg->wParam, pMsg->lParam);
// 	}
	return true;
}

void BooFileViewNode::DoInit()
{
	m_text->SetName(this->GetName());
}

void BooFileViewNode::SetFocus()
{
	m_text->SetFocus();
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
		static_cast<CControlUI*>(m_items[i])->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
	}
}

bool BooFileViewUI::OnNodeNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if( pMsg->sType == _T("createnode") )
	{
		int nIndex = GetFocusedNodeIndex(static_cast<BooFileViewNode*>(pMsg->pSender));
		if (-1 != nIndex)
		{
			BooFileViewNode* pNode = new BooFileViewNode();
			if (CREATENODE_NEXT == pMsg->wParam)
			{
				pNode->m_nIndent = static_cast<BooFileViewNode*>(GetItemAt(nIndex))->m_nIndent;
				CStdString strAttr;
				strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), pNode->m_nIndent);
				pNode->ApplyAttributeList(strAttr);
				int i=nIndex+1;
				for (; i<m_items.GetSize(); i++)
				{
					if (static_cast<BooFileViewNode*>(GetItemAt(i))->m_nIndent <= pNode->m_nIndent)
					{
						break;
					}
				}
				AddAt(pNode, i);
			}
			else if (CREATENODE_PREVIOUS== pMsg->wParam)
			{
				pNode->m_nIndent = static_cast<BooFileViewNode*>(GetItemAt(nIndex))->m_nIndent;
				CStdString strAttr;
				strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), pNode->m_nIndent);
				pNode->ApplyAttributeList(strAttr);
				AddAt(pNode, nIndex);
			}
			else if (CREATENODE_CHILD== pMsg->wParam)
			{
				BooFileViewNode* pParentNode = static_cast<BooFileViewNode*>(GetItemAt(nIndex));
				if (!pParentNode->m_bExpand)
				{
					ToggleNodeState(static_cast<BooFileViewNode*>(pParentNode));
				}
				pNode->m_nIndent = pParentNode->m_nIndent+1;
				CStdString strAttr;
				strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), pNode->m_nIndent);
				pNode->ApplyAttributeList(strAttr);
				AddAt(pNode, nIndex+1);
			}
			pNode->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
			pNode->SetFocus();
		}
	}
	else if ( pMsg->sType == _T("statebuttonclick"))
	{
		ToggleNodeState(static_cast<BooFileViewNode*>(pMsg->pSender));
	}
// 	else if( pMsg->sType == _T("setnodefocus") )
// 	{
// 		int a = 0;
// 	}
	return true;
}

void BooFileViewUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
}

int BooFileViewUI::GetFocusedNodeIndex(BooFileViewNode* pNode)
{
	int i=0;
	bool bFound = false;
	for (; i<m_items.GetSize(); i++)
	{
		if (m_items[i] == pNode)
		{
			bFound = true;
			break;
		}
	}
	if (bFound)
	{
		return i;
	}
	return -1;
}

bool BooFileViewUI::HasChildrenNode(BooFileViewNode* pNode)
{
	bool bHasChildren = false;
	int nIndex = GetFocusedNodeIndex(pNode);
	if (nIndex >= 0 && nIndex <= m_items.GetSize()-2) //最后一个节点必然没有子节点所以<= -2
	{
		if (static_cast<BooFileViewNode*>(m_items[nIndex+1])->m_nIndent > pNode->m_nIndent)
		{
			bHasChildren = true;
		}
		else
		{
			bHasChildren = false;
		}
	}
	return bHasChildren;
}

void BooFileViewUI::ToggleNodeState( BooFileViewNode* pNode )
{
	int nIndex = GetFocusedNodeIndex(pNode);
	//找到节点，并且节点不是最后一个
	if (nIndex >= 0 && nIndex < m_items.GetSize()-1)
	{
		//有子节点
		if (pNode->m_nIndent
			< static_cast<BooFileViewNode*>(m_items[nIndex+1])->m_nIndent)
		{
			bool bNewVisibleState = !static_cast<BooFileViewNode*>(m_items[nIndex+1])->IsVisible();
			for (int i=nIndex+1; 
				i<m_items.GetSize()
				&& static_cast<BooFileViewNode*>(m_items[i])->m_nIndent > pNode->m_nIndent;
			i++)
			{
				static_cast<BooFileViewNode*>(m_items[i])->SetVisible(bNewVisibleState);

				//如果这个节点处于收起状态，则跳过他的所有子节点。
				if (i<m_items.GetSize()-1 //保证不是最后一个节点
					&& static_cast<BooFileViewNode*>(m_items[i+1])->m_nIndent > static_cast<BooFileViewNode*>(m_items[i])->m_nIndent //有子节点
					&& !static_cast<BooFileViewNode*>(m_items[i])->m_bExpand) //子节点是收起的
				{
					int nCollapsedNodeIndex = i;
					for (;
						i<m_items.GetSize()-1
						&& static_cast<BooFileViewNode*>(m_items[i+1])->m_nIndent > static_cast<BooFileViewNode*>(m_items[nCollapsedNodeIndex])->m_nIndent;
					i++)
						;
				}
				//i--;//调整游标
			}
			pNode->m_bExpand = bNewVisibleState;
		}
	}
}