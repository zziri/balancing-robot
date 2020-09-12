// ChildSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Command_Center.h"
#include "Command_CenterDlg.h"
#include "ChildSocket.h"
#include "ListenSocket.h"

// CChildSocket

CChildSocket::CChildSocket()
	: m_pListenSocket(NULL)
	, pDlg(NULL)
{
}

CChildSocket::~CChildSocket()
{
}


// CChildSocket 멤버 함수


void CChildSocket::SetListenSocket(CAsyncSocket* pSocket)
{
	m_pListenSocket=pSocket;
}


void CChildSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
	CSocket::OnClose(nErrorCode);
}


void CChildSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// 보낼 수 있도록 플래그 SET
	CString temp = _T(""), strIPAddress = _T("");
	UINT uPortNumber = 0;
	GetPeerName(pDlg->strIPAddress, uPortNumber);
	int len;
	memset(&mcu2pc, 0, sizeof(mcu2pc));				/* 클리어 */
	if((len = Receive(&mcu2pc, sizeof(mcu2pc))) > 0){
		pDlg->isCanSend = true;
		pDlg->q.push(mcu2pc);
	}
	CSocket::OnReceive(nErrorCode);
}


void CChildSocket::Append2Graph(void)
{
	if(q.empty())
		return;
	MCU2PC temp = q.front();
	q.pop();
	double value[3] = {temp.monitoring.angle, 
		temp.monitoring.encoder.left, temp.monitoring.encoder.right};
	pDlg->pGraph_Angle->AppendPoints(value);								/* 같은 점을 두개 추가 */
	//pDlg->pGraph_Encoder->AppendPoints(value+1);							/* 그래프가 빠르게 지나가도록 */
	pDlg->pGraph_Angle->AppendPoints(value);
	//pDlg->pGraph_Encoder->AppendPoints(value+1);
}


void CChildSocket::delay_ms(clock_t n)
{
	clock_t start = clock();
	while(clock() - start < n);
}
