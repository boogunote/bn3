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
				(m_szRequest.cx != preqsz->rc.right ||	m_szRequest.cy != preqsz->rc.bottom + 4)) //注意，这里+4是为了留出上下2个像素的空白，不然上下就会显得太挤
			{
 				m_szRequest.cx = preqsz->rc.right;
 				m_szRequest.cy = preqsz->rc.bottom + 4;//注意，这里+4是为了留出上下2个像素的空白，不然上下就会显得太挤
				g_nTextHeight = m_szRequest.cy;
				m_pManager->SendNotify(this, _T("initbooview"), 0, 0, true); 
				//大家肯定觉得很奇怪，为什么要在这个富文本控件的OnTxNofty里面发出initbooview消息。
				//原因是，我必须要知道在当前的字体和字号下一行的高度是多少。这样我才能够确定文字块前面那个 +/- 号小方块的位置。
				//这个方法很脏，有没有什么好的方法，大家可以贡献一下。
				//还要注意一点，这个地方SendNotify一定要用异步方式发送。不然的话，在initbooview处理消息的时候，RemoveAll不能把这个测试控件移除
				//因为同步消息的传输，在RemoveAll的时候这个控件还没加入控件树
			}
		}
	}

private:
	SIZE m_szRequest;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BooFileViewUI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BooFileViewUI::BooFileViewUI() : m_nShiftSelectStart(-1)
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
	
	m_pManager->AddMessageFilter(this);
}

bool BooFileViewUI::OnNodeNotify(void* param)
{
	TNotifyUI* pMsg = (TNotifyUI*)param;

	if ( pMsg->sType == _T("statebuttonclick"))
	{
		ToggleNodeState(static_cast<BooFileViewNodeUI*>(pMsg->pSender));
	}
	else if ( pMsg->sType == _T("initbooview"))
	{
		this->RemoveAt(0);//删掉文字高度测试控件

		m_strBooFilePath = L"D:\\test.boo";

		HANDLE hFile = CreateFileW(m_strBooFilePath, GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD error = GetLastError();
		HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		BYTE* lpvFile = (BYTE *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);

		if (m_xml.LoadFromMem(lpvFile, dwFileSize))
		{
			CMarkupNode root = m_xml.GetRoot();
			if (root.IsValid())
			{
				BooFileViewNodeUI DummyNode;
				DummyNode.m_nIndent = -1;
				DummyNode.m_bExpand = true;
				LPCTSTR pstrName = root.GetAttributeName(0);
				LPCTSTR pstrValue = root.GetAttributeValue(0);
				int nVersion = _ttoi(pstrValue);
				if (nVersion<=7)
				{
					VisitNodeV7(root, &DummyNode, true);
				}
				else
				{
					VisitNodeV8(root, &DummyNode, true);
				}
			}
		}

		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);

		//CreateNode(0, 0);

	}
	else if( pMsg->sType == _T("selectmultinode") )
	{
		int nSelectIndex = GetItemIndex(pMsg->pSender);
		if (-1 == m_nShiftSelectStart)
		{
			m_nShiftSelectStart = nSelectIndex;
		}
		else
		{
			int nStart = min(m_nShiftSelectStart, nSelectIndex);
			int nEnd = max(m_nShiftSelectStart, nSelectIndex);
			for (int i=nStart; i<=nEnd; i++)
			{
				static_cast<BooFileViewNodeUI*>(m_items[i])->SetSelect(true);
			}
			//最后一个节点如果是收起状态的话，子节点都要选上
			if (static_cast<BooFileViewNodeUI*>(m_items[nEnd])->m_bHasChild
				&& !static_cast<BooFileViewNodeUI*>(m_items[nEnd])->m_bExpand)
			{
				for (int i=nEnd; i<m_items.GetSize() && static_cast<BooFileViewNodeUI*>(m_items[i])->m_nIndent > static_cast<BooFileViewNodeUI*>(m_items[nEnd])->m_nIndent; i++)
				{
					static_cast<BooFileViewNodeUI*>(m_items[i])->SetSelect(true);
				}
			}
			
		}
	}
	else if ( pMsg->sType == _T("setfocus"))
	{
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
		}
		else
		{
			m_nShiftSelectStart = GetItemIndex(pMsg->pSender);
		}
	}
	else if (pMsg->sType == _T("selectnode"))
	{
		BooFileViewNodeUI* pNode  = static_cast<BooFileViewNodeUI*>(pMsg->pSender);
		bool bSelected = !pNode->m_bSelected;
		pNode->SetSelect(bSelected);
		if (!pNode->m_bExpand)
		{
			for (int i=GetItemIndex(pNode); i<m_items.GetSize() && static_cast<BooFileViewNodeUI*>(m_items[i])->m_nIndent > pNode->m_nIndent; i++)
			{
				static_cast<BooFileViewNodeUI*>(m_items[i])->SetSelect(bSelected);
			}
		}
	}
	return true;
}

LRESULT BooFileViewUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (!IsVisible()) return 0;

	if (WM_KEYDOWN == uMsg)
	{
		switch (wParam)
		{
		case VK_DELETE:	OnDeleteKeyDown(bHandled); break;
		case VK_RETURN:	OnReturnKeyDown(bHandled); break;
		case VK_UP: OnUpKeyDown(); break;
		case VK_DOWN: OnDownKeyDown(); break;
		case VK_OEM_PLUS: OnPlusKeyDown(); break;
		case VK_OEM_MINUS: OnMinusKeyDown(); break;
		case 0x53: OnSKeyDown();break;
		default: break;
		}
	}
	else if (WM_LBUTTONDOWN == uMsg)
	{
		if (!((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000)))
		{
			CleanSelect();
		}
	}
	return 0;
}

void BooFileViewUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
}

BooFileViewNodeUI* BooFileViewUI::GetFocusedNode()
{
	BooFileViewNodeUI* pFocusedNode = NULL;
	BooTextFieldUI *pTextField = dynamic_cast<BooTextFieldUI *>(GetManager()->GetFocus());
	if (NULL != pTextField)
	{
		pFocusedNode = dynamic_cast<BooFileViewNodeUI *>(pTextField->GetParent());
	}
	return pFocusedNode;
}

bool BooFileViewUI::HasChildrenNode(BooFileViewNodeUI* pNode)
{
	bool bHasChildren = false;
	int nIndex = GetItemIndex(pNode);
	if (nIndex >= 0 && nIndex <= m_items.GetSize()-2) //最后一个节点必然没有子节点所以<= -2
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
	int nIndex = GetItemIndex(pNode);
	//找到节点，并且节点不是最后一个
	if (nIndex >= 0 && nIndex < m_items.GetSize()-1)
	{
		//有子节点
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

				//如果这个节点处于收起状态，则跳过他的所有子节点。
				if (i<m_items.GetSize()-1 //保证不是最后一个节点
					&& static_cast<BooFileViewNodeUI*>(m_items[i+1])->m_nIndent > static_cast<BooFileViewNodeUI*>(m_items[i])->m_nIndent //有子节点
					&& !static_cast<BooFileViewNodeUI*>(m_items[i])->m_bExpand) //子节点是收起的
				{
					int nCollapsedNodeIndex = i;
					for (;
						i<m_items.GetSize()-1
						&& static_cast<BooFileViewNodeUI*>(m_items[i+1])->m_nIndent > static_cast<BooFileViewNodeUI*>(m_items[nCollapsedNodeIndex])->m_nIndent;
					i++)
						;
				}
				//i--;//调整游标
			}
			pNode->m_bExpand = bNewVisibleState;
			pNode->UpdateStateButton();
		}
	}

	
}

void BooFileViewUI::CleanSelect()
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

BooFileViewNodeUI* BooFileViewUI::CreateNode(int nIndent, int nInsertAt)
{
	BooFileViewNodeUI* pNewNode = new BooFileViewNodeUI;
	CStdString strAttr;
	strAttr.Format(_T("width=\"0\" height=\"0\" textpadding=\"2,0,2,0\" align=\"wrap\" padding=\"2,2,2,2\" indent=\"%d\""), nIndent);
	pNewNode->ApplyAttributeList(strAttr);
	pNewNode->OnNotify += MakeDelegate(this, &BooFileViewUI::OnNodeNotify);
	pNewNode->SetHasChildren(false);
	if (nInsertAt<0)
	{
		this->Add(pNewNode);
	}
	else
	{
		this->AddAt(pNewNode, nInsertAt);
		m_nShiftSelectStart = nInsertAt;
	}
	
	return pNewNode;
}

void BooFileViewUI::OnDeleteKeyDown( bool& bHandled )
{
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		//下面是一个小技巧，把焦点所在文字块的节点也设成selected。然后在后面的for循环里面可以一起删掉
		BooTextFieldUI * pTextField = dynamic_cast<BooTextFieldUI *>(m_pManager->GetFocus());
		if (NULL != pTextField)
		{
			BooFileViewNodeUI *pNode = dynamic_cast<BooFileViewNodeUI *>(pTextField->GetParent());
			if (NULL != pNode)
			{
				pNode->m_bSelected = true;
			}
		}
		//删除所有select的节点
		for (int i=0; i<m_items.GetSize(); i++)
		{
			if (static_cast<BooFileViewNodeUI*>(m_items[i])->m_bSelected)
			{
				if( m_bAutoDestroy ) {
					delete static_cast<BooFileViewNodeUI*>(m_items[i]);
				}
				m_items.Remove(i);
				i--;//删除后调整迭代计数器
			}
		}

		if (m_items.GetSize() == 0)
		{
			CreateNode(0,0);
		}

		NeedUpdate();

		bHandled = true;
	}
}

void BooFileViewUI::OnReturnKeyDown( bool& bHandled )
{
	if ((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000))
	{
		BooFileViewNodeUI* pFocusedNode = GetFocusedNode();
		if (pFocusedNode)
		{
			int nIndex = GetItemIndex(pFocusedNode);
			int nIndent = static_cast<BooFileViewNodeUI*>(GetItemAt(nIndex))->m_nIndent;
			BooFileViewNodeUI* pNode = NULL;
			if (-1 != nIndex)
			{
				if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
				{
					pNode = CreateNode(nIndent, nIndex);
					bHandled = true;
				}
				else if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					int i=nIndex+1;
					for (; i<m_items.GetSize(); i++)
					{
						if (static_cast<BooFileViewNodeUI*>(GetItemAt(i))->m_nIndent <= nIndent)
						{
							break;
						}
					}
					pNode = CreateNode(nIndent, i);
					bHandled = true;
				}
				else if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					BooFileViewNodeUI* pParentNode = static_cast<BooFileViewNodeUI*>(GetItemAt(nIndex));
					pParentNode->SetHasChildren(true);
					if (!pParentNode->m_bExpand)
					{
						ToggleNodeState(static_cast<BooFileViewNodeUI*>(pParentNode));
					}
					pNode = CreateNode(nIndent+1, nIndent+1);
					bHandled = true;
				}

				if (pNode)
				{
					pNode->SetFocus();
				}
			}
		}
	}
}

void BooFileViewUI::OnUpKeyDown()
{
	BooFileViewNodeUI* pFocusedNode = GetFocusedNode();
	if (pFocusedNode)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
		}
		else
		{
			POINT pt;
			::GetCaretPos(&pt);
			if (abs(pt.y - pFocusedNode->GetPos().top) < g_nTextHeight*0.5)
			{
				for (int i = GetItemIndex(pFocusedNode)-1; i>=0; i--)
				{
					if (static_cast<BooFileViewNodeUI*>(m_items[i])->IsVisible())
					{
						static_cast<BooFileViewNodeUI*>(m_items[i])->SetFocus();
						m_nShiftSelectStart = i;
						break;
					}
				}
			}
		}
	}
}

void BooFileViewUI::OnDownKeyDown()
{
	BooFileViewNodeUI* pFocusedNode = GetFocusedNode();
	if (pFocusedNode)
	{
		POINT pt;
		::GetCaretPos(&pt);
		if (abs(pt.y - pFocusedNode->GetPos().bottom) < g_nTextHeight*1.5)
		{
			for (int i = GetItemIndex(pFocusedNode)+1; i<m_items.GetSize(); i++)
			{
				if (static_cast<BooFileViewNodeUI*>(m_items[i])->IsVisible())
				{
					static_cast<BooFileViewNodeUI*>(m_items[i])->SetFocus();
					m_nShiftSelectStart = i;
					break;
				}
			}
		}
	}
}

void BooFileViewUI::OnPlusKeyDown()
{
	BooFileViewNodeUI* pFocusedNode = GetFocusedNode();
	if (pFocusedNode)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			int nNextIconIndex = pFocusedNode->m_nIconIndex+1;
			if (nNextIconIndex > 6) nNextIconIndex = -1;
			pFocusedNode->SetIconIndex(nNextIconIndex);
		}
	}
}

void BooFileViewUI::OnMinusKeyDown()
{
	BooFileViewNodeUI* pFocusedNode = GetFocusedNode();
	if (pFocusedNode)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			int nNextIconIndex = pFocusedNode->m_nIconIndex-1;
			if (nNextIconIndex <= -1) nNextIconIndex = 6;
			pFocusedNode->SetIconIndex(nNextIconIndex);
		}
	}
}

void BooFileViewUI::VisitNodeV7( CMarkupNode &root, BooFileViewNodeUI* pParent, bool bExpandChildren /*爷爷节点关闭的话孙子节点也要不显示*/ )
{
	LPCTSTR pstrClass = NULL;
	int nAttributes = 0;
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;
	for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() )
	{
		pstrClass = node.GetName();
		nAttributes = node.GetAttributeCount();
		BooFileViewNodeUI* pNewNode = CreateNode(pParent->m_nIndent+1, -1);
		for( int i = 0; i < nAttributes; i++ )
		{
			pstrName = node.GetAttributeName(i);
			pstrValue = node.GetAttributeValue(i);
			pNewNode->SetVisible(bExpandChildren);

			if( _tcscmp(pstrName, _T("content")) == 0 )
			{
				pNewNode->m_strContent = pstrValue;
				pNewNode->m_strContent.Replace(_T("&#xA;"), _T("\n"));
			}
			else if( _tcscmp(pstrName, _T("block")) == 0 )
			{
				if (_tcscmp(pstrValue, _T("narrow")) == 0 || _tcscmp(pstrValue, _T("wide")) == 0)
				{
					pNewNode->SetOneLine(false);
				}
				else
				{
					pNewNode->SetOneLine(true, true);
				}
			}
			else if( _tcscmp(pstrName, _T("icon")) == 0 ) 
			{
				int nIconIndex = -1;
				if ( _tcscmp(pstrValue, _T("none")) == 0 )
				{
					nIconIndex = -1;
				}
				else if (_tcscmp(pstrValue, _T("none")) == 0)
				{
					nIconIndex = 0;
				}
				else if (_tcscmp(pstrValue, _T("flag")) == 0)
				{
					nIconIndex = 1;
				}
				else if (_tcscmp(pstrValue, _T("tick")) == 0)
				{
					nIconIndex = 2;
				}
				else if (_tcscmp(pstrValue, _T("cross")) == 0)
				{
					nIconIndex = 3;
				}
				else if (_tcscmp(pstrValue, _T("star")) == 0)
				{
					nIconIndex = 4;
				}
				else if (_tcscmp(pstrValue, _T("question")) == 0)
				{
					nIconIndex = 5;
				}
				else if (_tcscmp(pstrValue, _T("warning")) == 0)
				{
					nIconIndex = 6;
				}
				else if (_tcscmp(pstrValue, _T("idea")) == 0)
				{
					nIconIndex = 7;
				}
				pNewNode->SetIconIndex(nIconIndex);
			}
			else if( _tcscmp(pstrName, _T("branch")) == 0 ) 
			{
				if (_tcscmp(pstrValue, _T("open")) == 0)
				{
					pNewNode->m_bExpand = true;
				}
				else if (_tcscmp(pstrValue, _T("close")) == 0)
				{
					pNewNode->m_bExpand = false;
				}
			}
			else if( _tcscmp(pstrName, _T("IsBold")) == 0 ) 
			{
				if (_tcscmp(pstrValue, _T("true")) == 0)
				{
					pNewNode->SetBold(true);
				}
				else
				{
					pNewNode->SetBold(false);
				}
			}
			else if( _tcscmp(pstrName, _T("TextColor")) == 0 ) 
			{
				pNewNode->SetTextColorV7(pstrValue);
			}
			else if( _tcscmp(pstrName, _T("BkgrdColor")) == 0 ) 
			{
				pNewNode->SetBkColorV7(pstrValue);
			}
			

		}
		if (node.HasChildren())
		{
			pNewNode->SetHasChildren(true);
			VisitNodeV7(node, pNewNode, pNewNode->m_bExpand && bExpandChildren);
		}
		else
		{
			pNewNode->SetHasChildren(false);
		}
	}
}

void BooFileViewUI::OnSKeyDown()
{
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		CStdString strXml = L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"\
							L"<root version=\"8\">\n";
		
		for (int nIndex = 0; nIndex<m_items.GetSize(); nIndex++)
		{
			CStdString strNodeXml;
			SerializeNode(nIndex, strNodeXml);
			strXml += strNodeXml;
		}
		strXml += L"</root>";
		
		HANDLE hFile = CreateFileW(m_strBooFilePath, GENERIC_WRITE,  0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			DWORD dwBytesWritten = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)strXml, -1, 0, 0, NULL, NULL);
			char* utf8buf = new char[dwBytesWritten];
			int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)strXml, -1, utf8buf, dwBytesWritten, NULL, NULL);
			SetFilePointer(hFile, 0, NULL, FILE_END);
			DWORD dwFileBytesWritten;
			WriteFile(hFile, utf8buf, lBytesWritten-1, &dwFileBytesWritten, NULL);
			CloseHandle(hFile);
		}
	}
}

void BooFileViewUI::SerializeNode(int& nIndex, CStdString& strXml)
{
	BooFileViewNodeUI* pNode = static_cast<BooFileViewNodeUI*>(m_items[nIndex]);
	CStdString strContent;
	pNode->GetContent(strContent);
	strContent.Replace(L"\n", L"&#xA;");
	strContent.Replace(L"\r", L"&#xA;");
	strContent.Replace(L"<", L"&lt;");
	strContent.Replace(L">", L"&gt;");
	strContent.Replace(L"&", L"&amp;");
	strContent.Replace(L"'", L"&apos;");
	strContent.Replace(L"\"", L"&quot;");
	strXml += L"<item content=\"";
	strXml += strContent; //这里没有直接format的原因是这个CStdString的format buffer 只有1024字节。。。
	CStdString strTemp;
	strTemp.Format(L"\" icon=\"%d\" expand=\"%d\" oneline=\"%d\" bold=\"%d\" textcolor=\"#%08X\" bkcolor=\"#%08X\"",
		pNode->m_nIconIndex, pNode->m_bExpand, pNode->m_bOneLine, pNode->m_bBold, pNode->GetTextFieldTextColor(), pNode->GetTextFieldBkColor());
	strXml += strTemp;
	//是否有子节点
	if (nIndex+1<m_items.GetSize() && (static_cast<BooFileViewNodeUI*>(m_items[nIndex+1])->m_nIndent > pNode->m_nIndent))
	{
		strXml += L">";
		for (nIndex++; nIndex<m_items.GetSize() && static_cast<BooFileViewNodeUI*>(m_items[nIndex])->m_nIndent > pNode->m_nIndent; nIndex++)
		{
			CStdString strNodeXml;
			SerializeNode(nIndex, strNodeXml);
			strXml += strNodeXml;
		}
		nIndex--;//修正一下
		strXml += L"</item>";
	}
	else
	{
		strXml += L"/>";
	}
	
	
}

void BooFileViewUI::VisitNodeV8( CMarkupNode &root, BooFileViewNodeUI* pParent, bool bExpandChildren )
{
	LPCTSTR pstrClass = NULL;
	int nAttributes = 0;
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;
	for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() )
	{
		pstrClass = node.GetName();
		nAttributes = node.GetAttributeCount();
		BooFileViewNodeUI* pNewNode = CreateNode(pParent->m_nIndent+1, -1);
		for( int i = 0; i < nAttributes; i++ )
		{
			pstrName = node.GetAttributeName(i);
			pstrValue = node.GetAttributeValue(i);
			pNewNode->SetVisible(bExpandChildren);

			if( _tcscmp(pstrName, _T("content")) == 0 )
			{
				pNewNode->m_strContent = pstrValue;
				pNewNode->m_strContent.Replace(_T("&#xA;"), _T("\n"));
			}
			else if( _tcscmp(pstrName, _T("oneline")) == 0 )
			{
				int nValue = _ttoi(pstrValue); 
				bool bValue = nValue==0?false:true;
				if (!bValue)
					pNewNode->SetOneLine(bValue);
				else
					pNewNode->SetOneLine(bValue, true);//如果是收缩文字块，需要进行一些处理，所以第二个参数要为 true
			}
			else if( _tcscmp(pstrName, _T("icon")) == 0 ) 
			{
				int nValue = _ttoi(pstrValue);
				pNewNode->SetIconIndex(nValue);
			}
			else if( _tcscmp(pstrName, _T("expand")) == 0 ) 
			{
				pNewNode->m_bExpand = _ttoi(pstrValue)==0?false:true;
			}
			else if( _tcscmp(pstrName, _T("bold")) == 0 ) 
			{
				pNewNode->SetBold(_ttoi(pstrValue)==0?false:true);
			}
			else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) 
			{
				pNewNode->SetTextColor(pstrValue);
			}
			else if( _tcscmp(pstrName, _T("bkcolor")) == 0 ) 
			{
				pNewNode->SetBkColor(pstrValue);
			}


		}
		if (node.HasChildren())
		{
			pNewNode->SetHasChildren(true);
			VisitNodeV8(node, pNewNode, pNewNode->m_bExpand && bExpandChildren);
		}
		else
		{
			pNewNode->SetHasChildren(false);
		}
	}
}