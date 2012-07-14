#include "stdafx.h"
#include "BooTextFieldUI.h"
#include "BooFileViewNodeUI.h"
#include "BooFileViewUI.h"

int g_nTextHeight = 0;

class BooTestRichEditUI : public CRichEditUI
{
public:
	BooTestRichEditUI(){}

	LPCTSTR GetClass() const
	{
		return _T("BooTestRichEditUI");
	}
	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("BooTestRichEdit")) == 0 ) return static_cast<BooTestRichEditUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void DoInit()
	{
		__super::DoInit();
		this->SetEventMask(ENM_REQUESTRESIZE|ENM_LINK);
		this->SetText(L".");
		this->SetText(L"");
	}

	void OnTxNotify(DWORD iNotify, void *pv)
	{
		if( iNotify == EN_REQUESTRESIZE )
		{
			REQRESIZE *preqsz = (REQRESIZE *)pv;
			if (0 != preqsz->rc.bottom &&
				(m_szRequest.cx != preqsz->rc.right ||	m_szRequest.cy != preqsz->rc.bottom + 4)) //ע�⣬����+4��Ϊ����������2�����صĿհף���Ȼ���¾ͻ��Ե�̫��
			{
 				m_szRequest.cx = preqsz->rc.right;
 				m_szRequest.cy = preqsz->rc.bottom + 4;//ע�⣬����+4��Ϊ����������2�����صĿհף���Ȼ���¾ͻ��Ե�̫��
				g_nTextHeight = m_szRequest.cy;
				m_pManager->SendNotify(this, _T("initbooview"), 0, 0, true); 
				//��ҿ϶����ú���֣�ΪʲôҪ��������ı��ؼ���OnTxNofty���淢��initbooview��Ϣ��
				//ԭ���ǣ��ұ���Ҫ֪���ڵ�ǰ��������ֺ���һ�еĸ߶��Ƕ��١������Ҳ��ܹ�ȷ�����ֿ�ǰ���Ǹ� +/- ��С�����λ�á�
				//����������࣬��û��ʲô�õķ�������ҿ��Թ���һ�¡�
				//��Ҫע��һ�㣬����ط�SendNotifyһ��Ҫ���첽��ʽ���͡���Ȼ�Ļ�����initbooview������Ϣ��ʱ��RemoveAll���ܰ�������Կؼ��Ƴ�
				//��Ϊͬ����Ϣ�Ĵ��䣬��RemoveAll��ʱ������ؼ���û����ؼ���
			}
		}
	}

private:
	SIZE m_szRequest;
};


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
	BooTestRichEditUI* pTest = new BooTestRichEditUI;
	pTest->ApplyAttributeList(_T("width=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FFEEEEEE\" bordersize=\"1\" focusbordercolor=\"#FF0000FF\" inset=\"2,2,2,2\""));
	pTest->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
	this->AddAt(pTest, 0);
	
// 	for (int i=0; i<m_items.GetSize(); i++)
// 	{
// 		static_cast<CControlUI*>(m_items[i])->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
// 	}
}

bool BooFileViewUI::OnNodeNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;
	if( pMsg->sType == _T("createnode") )
	{
		int nIndex = GetFocusedNodeIndex(static_cast<BooFileViewNodeUI*>(pMsg->pSender));
		if (-1 != nIndex)
		{
			BooFileViewNodeUI* pNode = new BooFileViewNodeUI();
			if (CREATENODE_NEXT == pMsg->wParam)
			{
				pNode->m_nIndent = static_cast<BooFileViewNodeUI*>(GetItemAt(nIndex))->m_nIndent;
				CStdString strAttr;
				strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), pNode->m_nIndent);
				pNode->ApplyAttributeList(strAttr);
				int i=nIndex+1;
				for (; i<m_items.GetSize(); i++)
				{
					if (static_cast<BooFileViewNodeUI*>(GetItemAt(i))->m_nIndent <= pNode->m_nIndent)
					{
						break;
					}
				}
				AddAt(pNode, i);
			}
			else if (CREATENODE_PREVIOUS== pMsg->wParam)
			{
				pNode->m_nIndent = static_cast<BooFileViewNodeUI*>(GetItemAt(nIndex))->m_nIndent;
				CStdString strAttr;
				strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), pNode->m_nIndent);
				pNode->ApplyAttributeList(strAttr);
				AddAt(pNode, nIndex);
			}
			else if (CREATENODE_CHILD== pMsg->wParam)
			{
				BooFileViewNodeUI* pParentNode = static_cast<BooFileViewNodeUI*>(GetItemAt(nIndex));
				pParentNode->m_bHasChild = true;
				pParentNode->UpdateStateButton();
				if (!pParentNode->m_bExpand)
				{
					ToggleNodeState(static_cast<BooFileViewNodeUI*>(pParentNode));
				}
				pNode->m_nIndent = pParentNode->m_nIndent+1;
				CStdString strAttr;
				strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), pNode->m_nIndent);
				pNode->ApplyAttributeList(strAttr);
				AddAt(pNode, nIndex+1);
			}
			pNode->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
			pNode->UpdateStateButton();
			pNode->SetFocus();
		}
	}
	else if ( pMsg->sType == _T("statebuttonclick"))
	{
		ToggleNodeState(static_cast<BooFileViewNodeUI*>(pMsg->pSender));
	}
	else if ( pMsg->sType == _T("initbooview"))
	{
		this->RemoveAt(0);//ɾ�����ָ߶Ȳ��Կؼ�

		BooFileViewNodeUI* pFirestNode = new BooFileViewNodeUI;
		pFirestNode->ApplyAttributeList(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"0\""));
		pFirestNode->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
		pFirestNode->m_bHasChild = false;
		pFirestNode->UpdateStateButton();
		this->Add(pFirestNode);
	}
	else if (pMsg->sType == _T("cleanselect"))
	{
		for (int i=0; i<m_items.GetSize(); i++)
		{
			if (static_cast<BooFileViewNodeUI*>(m_items[i])->m_bSelected)
			{
				static_cast<BooFileViewNodeUI*>(m_items[i])->m_bSelected = false;
				static_cast<BooFileViewNodeUI*>(m_items[i])->SetSelect(false);
			}
		}
	}
	else if (pMsg->sType == _T("movefocus"))
	{
		if (pMsg->wParam == 0)
		{
			for (int i = GetItemIndex(pMsg->pSender)-1; i>=0; i--)
			{
				if (static_cast<BooFileViewNodeUI*>(m_items[i])->IsVisible())
				{
					static_cast<BooFileViewNodeUI*>(m_items[i])->SetFocus();
					break;
				}
			}
		}
		else
		{
			for (int i = GetItemIndex(pMsg->pSender)+1; i<m_items.GetSize(); i++)
			{
				if (static_cast<BooFileViewNodeUI*>(m_items[i])->IsVisible())
				{
					static_cast<BooFileViewNodeUI*>(m_items[i])->SetFocus();
					break;
				}
			}
		}
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

int BooFileViewUI::GetFocusedNodeIndex(BooFileViewNodeUI* pNode)
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

bool BooFileViewUI::HasChildrenNode(BooFileViewNodeUI* pNode)
{
	bool bHasChildren = false;
	int nIndex = GetFocusedNodeIndex(pNode);
	if (nIndex >= 0 && nIndex <= m_items.GetSize()-2) //���һ���ڵ��Ȼû���ӽڵ�����<= -2
	{
		if (static_cast<BooFileViewNodeUI*>(m_items[nIndex+1])->m_nIndent > pNode->m_nIndent)
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

void BooFileViewUI::ToggleNodeState( BooFileViewNodeUI* pNode )
{
	int nIndex = GetFocusedNodeIndex(pNode);
	//�ҵ��ڵ㣬���ҽڵ㲻�����һ��
	if (nIndex >= 0 && nIndex < m_items.GetSize()-1)
	{
		//���ӽڵ�
		if (pNode->m_nIndent
			< static_cast<BooFileViewNodeUI*>(m_items[nIndex+1])->m_nIndent)
		{
			bool bNewVisibleState = !static_cast<BooFileViewNodeUI*>(m_items[nIndex+1])->IsVisible();
			for (int i=nIndex+1; 
				i<m_items.GetSize()
				&& static_cast<BooFileViewNodeUI*>(m_items[i])->m_nIndent > pNode->m_nIndent;
			i++)
			{
				static_cast<BooFileViewNodeUI*>(m_items[i])->SetVisible(bNewVisibleState);

				//�������ڵ㴦������״̬�����������������ӽڵ㡣
				if (i<m_items.GetSize()-1 //��֤�������һ���ڵ�
					&& static_cast<BooFileViewNodeUI*>(m_items[i+1])->m_nIndent > static_cast<BooFileViewNodeUI*>(m_items[i])->m_nIndent //���ӽڵ�
					&& !static_cast<BooFileViewNodeUI*>(m_items[i])->m_bExpand) //�ӽڵ��������
				{
					int nCollapsedNodeIndex = i;
					for (;
						i<m_items.GetSize()-1
						&& static_cast<BooFileViewNodeUI*>(m_items[i+1])->m_nIndent > static_cast<BooFileViewNodeUI*>(m_items[nCollapsedNodeIndex])->m_nIndent;
					i++)
						;
				}
				//i--;//�����α�
			}
			pNode->m_bExpand = bNewVisibleState;
			pNode->UpdateStateButton();
		}
	}

	
}