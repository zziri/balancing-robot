// ListenSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Command_Center.h"
#include "ListenSocket.h"


// CListenSocket

CListenSocket::CListenSocket()
	: pDlg(NULL)
	, isCanSend(false)
{
}

CListenSocket::~CListenSocket()
{
}


// CListenSocket 멤버 함수

#include "Command_CenterDlg.h"
void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CChildSocket* pChild = new CChildSocket;
	pChild->pDlg = this->pDlg;
	BOOL check = Accept(*pChild);
	if(check == FALSE){
		delete pChild;
		AfxMessageBox(_T("접속허용실패"));
		return;
	}
	pChild->SetListenSocket(this);
	ChildSocket_List.AddTail(pChild);
	CCommand_CenterDlg* pMain = (CCommand_CenterDlg*)AfxGetMainWnd();
	pMain->m_List_State.AddString(_T("서버접속허용"));
	pMain->m_List_State.SetCurSel(pMain->m_List_State.GetCount()-1);
	CAsyncSocket::OnAccept(nErrorCode);
}


void CListenSocket::CloseClientSocket(CSocket* pChild)
{
	POSITION pos;
	pos = ChildSocket_List.Find(pChild);
	if(pos != NULL){
		if(pChild != NULL){
			pChild->ShutDown();
			pChild->Close();
		}
		ChildSocket_List.RemoveAt(pos);
		delete pChild;
	}

}


void CListenSocket::BroadCast(PC2MCU* pszBuffer, int len)
{
	//if(pszBuffer->command == 'C')	// 'C' mean Callback
	//	return;
	if(pDlg->isCanSend){
		POSITION pos;
		pos = ChildSocket_List.GetHeadPosition();
		CChildSocket* pChild = NULL;
		while(pos != NULL){
			pChild = (CChildSocket*)ChildSocket_List.GetNext(pos);
			if(pChild != NULL)
				pChild->Send(pszBuffer, len);
		}
		pDlg->isCanSend = false;
	}
}
