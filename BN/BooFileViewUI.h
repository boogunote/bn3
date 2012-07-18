#pragma once

#define CREATENODE_PREVIOUS	0
#define CREATENODE_NEXT		1
#define CREATENODE_CHILD	2

class BooFileViewUI : public CVerticalLayoutUI, public IMessageFilterUI
{
public:
	BooFileViewUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void DoInit();
	void SetPos(RECT rc);
	bool OnNodeNotify(void* param);

	void VisitNodeV7( CMarkupNode &root, BooFileViewNodeUI* pParent, bool bExpandChildren);
	void VisitNodeV8( CMarkupNode &root, BooFileViewNodeUI* pParent, bool bExpandChildren );
	BooFileViewNodeUI* CreateNode(int nIndent, int nInsertAt);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void OnPlusKeyDown();
	void OnMinusKeyDown();
	void OnDownKeyDown();
	void OnUpKeyDown();
	void OnReturnKeyDown( bool& bHandled );
	void OnDeleteKeyDown( bool& bHandled );
	void OnSKeyDown();
	void CleanSelect();
	void ToggleNodeState( BooFileViewNodeUI* pSenderNode );
	BooFileViewNodeUI* GetFocusedNode();
	bool HasChildrenNode(BooFileViewNodeUI* pNode);
	void SerializeNode(int& nIndex, CStdString& strXml);
public:
	int m_nShiftSelectStart;
	CStdString m_strBooFilePath;
	CMarkup m_xml;
};