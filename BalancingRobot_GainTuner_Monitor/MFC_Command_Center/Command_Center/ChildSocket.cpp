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
	//AfxBeginThread(Thread_AppendPoints, this);
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
		//CCommand_CenterDlg* pMain = (CCommand_CenterDlg*)AfxGetMainWnd();
		pDlg->isCanSend = true;
		//pDlg->flags.receive = true;
		pDlg->q.push(mcu2pc);
		//pDlg->pMcu2pc = &mcu2pc;
		//if(mcu2pc.command & 0x80){					/* MSB는 콜백, 콜백은 브로드캐스팅 하지 않음 */
		//	if(mcu2pc.command & 0x1F){				/* 주행중 */
		//		temp.Format(_T("%s: Robot is Driving,, "), strIPAddress);
		//		if(mcu2pc.command & 0x08){					/* 우회전 */
		//			temp += "Right";
		//		}
		//		else if(mcu2pc.command & 0x04){				/* 좌회전 */
		//			temp += "Left";
		//		}
		//		else if(mcu2pc.command & 0x02){				/* 전진 */
		//			temp += "Forward";
		//		}
		//		else if(mcu2pc.command & 0x01){				/* 정지 */
		//			temp += "Stop";
		//		}
		//		else if(mcu2pc.command & 0x10){				/* 후진 */
		//			temp += "Backward";
		//		}
		//	}
		//	else{
		//		temp.Format(_T("%s: Tuning Complete,, "), strIPAddress);
		//	}
		//	pMain->m_List_State.AddString(temp);
		//	pMain->m_List_State.SetCurSel(pMain->m_List_State.GetCount()-1);
		//}
		//else if(mcu2pc.command & 0x40){						/* 준비 됐다는 메세지 */
		//	double value[3] = {mcu2pc.monitoring.angle,};
		//	pDlg->pGraph_Angle->AppendPoints(value);
		//	temp.Format(_T("%s: Robot is Ready,, "), strIPAddress);
		//	pMain->m_List_State.AddString(temp);
		//	pMain->m_List_State.SetCurSel(pMain->m_List_State.GetCount()-1);
		//}
		//if(mcu2pc.command == 0x00){
		//	//AfxBeginThread(Thread_AppendPoints, this);				/* 끊길시에 어플리케이션이 먹통이되는 현상 막기위해 쓰레드로 동작 */
		//	pDlg->q.push(mcu2pc);
		//}
	}
	CSocket::OnReceive(nErrorCode);
}


UINT CChildSocket::Thread_AppendPoints(LPVOID pParam)				/* 그래프에 점을 추가하는 쓰레드 */
{
	CChildSocket* pChildSocket = (CChildSocket*)pParam;
	while(1){
		pChildSocket->delay_ms(50);
		pChildSocket->Append2Graph();
	}

	return 0;
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
