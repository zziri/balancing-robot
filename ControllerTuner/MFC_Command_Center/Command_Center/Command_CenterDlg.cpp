
// Command_CenterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Command_Center.h"
#include "Command_CenterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCommand_CenterDlg 대화 상자



CCommand_CenterDlg::CCommand_CenterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommand_CenterDlg::IDD, pParent)
	, pListenSocket(NULL)
	, Gain_P(0)
	, Gain_I(0)
	, Gain_D(0)
	, pGraph_Angle(NULL)
	, I_Centroid(0)
	, P_Centroid(0)
	, D_Centroid(0)
	, m_speed_cmd(0)
	, P_Speed(0)
	, isCanSend(false)
	, I_Speed(0)
	, pGraph_Encoder(NULL)
	, strIPAddress(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	memset(&flags, 0, sizeof(Flag));
}

void CCommand_CenterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATE, m_List_State);
	DDX_Text(pDX, IDC_EDIT_PGAIN, Gain_P);
	DDX_Text(pDX, IDC_EDIT_IGAIN, Gain_I);
	DDX_Text(pDX, IDC_EDIT_DGAIN, Gain_D);
	DDX_Text(pDX, IDC_EDIT_ICEN, I_Centroid);
	DDX_Text(pDX, IDC_EDIT_PCEN, P_Centroid);
	DDX_Text(pDX, IDC_EDIT_DCEN, D_Centroid);
	DDX_Control(pDX, IDC_CHECK_F, m_BtnF);
	DDX_Control(pDX, IDC_CHECK_L, m_BtnL);
	DDX_Control(pDX, IDC_CHECK_R, m_BtnR);
	DDX_Control(pDX, IDC_CHECK_B, m_BtnB);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_speed_cmd);
	DDX_Text(pDX, IDC_EDIT_PSPEED, P_Speed);
	DDX_Text(pDX, IDC_EDIT_ISPEED, I_Speed);
}

BEGIN_MESSAGE_MAP(CCommand_CenterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CCommand_CenterDlg::OnBnClickedButtonSend)
	ON_WM_TIMER()
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CCommand_CenterDlg 메시지 처리기
BOOL CCommand_CenterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// Configuration Socket
	pListenSocket = new CListenSocket;
	pListenSocket->pDlg = this;
	if(pListenSocket->Create(PORT_NUMBER, SOCK_STREAM)){
		if(pListenSocket->Listen()){

		}else{
			AfxMessageBox(_T("Failed to LISTEN"));
		}
	}
	else{
		AfxMessageBox(_T("Failed to create socket for listen."));
	}

	// Configuration Graph
	GraphConfiguration();
	//SetTimer(TIM_ID_GRAPH, 1, NULL);						/* OnReceive에서 대신 처리 */
	// Edit Control Init
	SetTimer(TIM_RECEIVE, 1, NULL);
	m_speed_cmd = DEFAULT_DRIVE_SPEED;
	UpdateData(FALSE);
	// Flag Init
	flags.button_clicked = false;
	if(!(ReadGains())){										/* 이전에 저장된 게인값 읽기 */
		AfxMessageBox(_T("Cannot read gains from file"));
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCommand_CenterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCommand_CenterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCommand_CenterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCommand_CenterDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	POSITION pos;
	pos=pListenSocket->ChildSocket_List.GetHeadPosition();
	CChildSocket* pChild = NULL;
	while(pos != NULL){
		pChild=(CChildSocket*)pListenSocket->ChildSocket_List.GetNext(pos);
		if(pChild != NULL){
			pChild->ShutDown();
			pChild->Close();
			delete pChild;
		}
	}
	pListenSocket->ShutDown();
	pListenSocket->Close();
	UpdateData(TRUE);																				/* 에디트 컨트롤에 있는 값을 변수에 복사 */
	Gain gain = {Gain_P, Gain_I, Gain_D, P_Centroid, I_Centroid, D_Centroid, P_Speed, I_Speed};		/* 게인값 복사 */
	if(!(SaveGains(gain))){																			/* 종료전 게인값 저장 */
		AfxMessageBox(_T("Cannot save gain in file"));
	}
	delete pGraph_Angle;
	//delete pGraph_Encoder;
}

void CCommand_CenterDlg::OnBnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	memset(&pc2mcu, 0, sizeof(pc2mcu));						/* 클리어 */
	pc2mcu.gain.pPosture = Gain_P;
	pc2mcu.gain.iPosture = Gain_I;
	pc2mcu.gain.dPosture = Gain_D;
	pc2mcu.gain.pCentroid = P_Centroid;
	pc2mcu.gain.iCentroid = I_Centroid;
	pc2mcu.gain.dCentroid = D_Centroid;
	pc2mcu.gain.pPosition = P_Speed;
	pc2mcu.gain.iPosition = I_Speed;
	pc2mcu.drive.dir = (char)TUNING;
	pc2mcu.drive.speed = 0;
	CreateCheckSum_PC2MCU();								/* 체크섬 필드 데이터 생성 */
	pListenSocket->BroadCast(&pc2mcu, sizeof(pc2mcu));		/* 클라이언트로 전송 */
	if(!(SaveGains(pc2mcu.gain))){							/* 그때그때 게인값 저장 */
		AfxMessageBox(_T("Cannot save gain in file"));
	}
	memset(&pc2mcu, 0, sizeof(pc2mcu));						/* 클리어 */
}

void CCommand_CenterDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	this->OnBnClickedButtonSend();
	//CDialogEx::OnOK();
}

void CCommand_CenterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//if(nIDEvent == TIM_ID_GRAPH){
	//	double t = (double)GetTickCount()/1000.0;
	//	double value[3] = {5.0*cos(t), 5.0*sin(t), 5.0*tan(t) };
	//	pGraph_Angle->AppendPoints(value);
	//}
	//else if(nIDEvent == TIM_ID_DRAW_GRAPH){
	//	pListenSocket->pChildSocket->Append2Graph();
	//}
	if(nIDEvent == TIM_RECEIVE){				// 로봇에서 어떤 메세지 도착한것을 처리
		if(!q.empty()){
			CString temp;
			MCU2PC mcu2pc = q.front();	q.pop();
			if(mcu2pc.command & 0x80){					/* MSB는 콜백, 콜백은 브로드캐스팅 하지 않음 */
				if(mcu2pc.command & 0x1F){				/* 주행중 */
					temp.Format(_T("%s: Robot is Driving,, "), strIPAddress);
					if(mcu2pc.command & 0x08){					/* 우회전 */
						temp += "Right";
					}
					else if(mcu2pc.command & 0x04){				/* 좌회전 */
						temp += "Left";
					}
					else if(mcu2pc.command & 0x02){				/* 전진 */
						temp += "Forward";
					}
					else if(mcu2pc.command & 0x01){				/* 정지 */
						temp += "Stop";
					}
					else if(mcu2pc.command & 0x10){				/* 후진 */
						temp += "Backward";
					}
				}
				else{
					temp.Format(_T("%s: Tuning Complete,, "), strIPAddress);
				}
				m_List_State.AddString(temp);
				m_List_State.SetCurSel(m_List_State.GetCount()-1);
			}
			else if(mcu2pc.command & 0x40){						/* 준비 됐다는 메세지 */
				double value[3] = {mcu2pc.monitoring.angle,};
				pGraph_Angle->AppendPoints(value);
				temp.Format(_T("%s: Robot is Ready,, "), strIPAddress);
				m_List_State.AddString(temp);
				m_List_State.SetCurSel(m_List_State.GetCount()-1);
			}
			else if(mcu2pc.command == 0x00){
				double value[3] = {mcu2pc.monitoring.angle, 
					mcu2pc.monitoring.encoder.left, mcu2pc.monitoring.encoder.right};
				//for(int i=0; i<3; i++){
				//	pGraph_Angle->AppendPoints(value);								/* 같은 점을 두개 추가 */
				//	pGraph_Encoder->AppendPoints(value+1);							/* 그래프가 빠르게 지나가도록 */
				//}
				pGraph_Angle->AppendPoints(value);
				pGraph_Encoder->AppendPoints(value+1);
				pGraph_Angle->AppendPoints(value);
				pGraph_Encoder->AppendPoints(value+1);
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CCommand_CenterDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN){
		if(!CheckDriveClicked()){
			SetDriveClicked(WM_KEYDOWN);
			switch (pMsg->wParam)
			{
			case VK_LEFT:
				m_BtnL.SetCheck(true);
				DriveCommand(LEFT);
				break;
			case VK_RIGHT:
				m_BtnR.SetCheck(true);
				DriveCommand(RIGHT);
				break;
			case VK_UP:
				m_BtnF.SetCheck(true);
				DriveCommand(FORWARD);
				break;
			case VK_DOWN:
				m_BtnB.SetCheck(true);
				DriveCommand(BACKWARD);
				break;
			default:
				break;
			}
		}
	}
	else if(pMsg->message == WM_KEYUP){
		SetDriveClicked(WM_KEYUP);
		switch (pMsg->wParam)
		{
		case VK_LEFT:
			m_BtnL.SetCheck(false);
			DriveCommand(STOP);
			break;
		case VK_RIGHT:
			m_BtnR.SetCheck(false);
			DriveCommand(STOP);
			break;
		case VK_UP:
			m_BtnF.SetCheck(false);
			DriveCommand(STOP);
			break;
		case VK_DOWN:
			m_BtnB.SetCheck(false);
			DriveCommand(STOP);
			break;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

bool CCommand_CenterDlg::CheckDriveClicked(void)
{
	return flags.button_clicked;
}

void CCommand_CenterDlg::SetDriveClicked(int mode)
{
	switch (mode)
	{
	case WM_KEYUP:
		flags.button_clicked = false;
		break;
	case WM_KEYDOWN:
		flags.button_clicked = true;
		break;
	default:
		break;
	}
}


bool CCommand_CenterDlg::ReadGains(void)
{
	// Variables
	FILE *pFile;
	Gain gain;
	// File Open
	if(!(pFile = fopen("gain.txt", "r")))
		return false;
	// Get gains
	if((fscanf(pFile, "%lf %lf %lf %lf %lf %lf %lf", &gain.pPosture, &gain.iPosture, &gain.dPosture, 
		&gain.pCentroid, &gain.iCentroid, &gain.dCentroid, &gain.pPosition)) == EOF)
		return false;
	// Output to Edit Control
	Gain_P = gain.pPosture;
	Gain_I = gain.iPosture;
	Gain_D = gain.dPosture;
	P_Centroid = gain.pCentroid;
	I_Centroid = gain.iCentroid;
	D_Centroid = gain.dCentroid;	
	P_Speed = gain.pPosition;
	UpdateData(FALSE);
	// close file
	fclose(pFile);
	// end..
	return true;
}


bool CCommand_CenterDlg::SaveGains(Gain gain)
{
	// Variables
	FILE *pFile;
	// file open
	if(!(pFile = fopen("C:\\temp\\gain.txt", "w")))
		return false;
	// Save gain values in file
	fprintf(pFile, "%f\n", gain.pPosture);
	fprintf(pFile, "%f\n", gain.iPosture);
	fprintf(pFile, "%f\n", gain.dPosture);
	fprintf(pFile, "%f\n", gain.pCentroid);
	fprintf(pFile, "%f\n", gain.iCentroid);
	fprintf(pFile, "%f\n", gain.dCentroid);
	fprintf(pFile, "%f\n", gain.pPosition);
	// close file
	fclose(pFile);
	// end..
	return true;
}


void CCommand_CenterDlg::GraphConfiguration(void)
{
	CRect Graph_Rect_Angle, Graph_Rect_Encoder;
	GetDlgItem(IDC_STATIC_GRAPH_ENGLE)->GetWindowRect(Graph_Rect_Angle);							/* CRect 설정 */
	GetDlgItem(IDC_STATIC_GRAPH_ENCODER)->GetWindowRect(Graph_Rect_Encoder);
	ScreenToClient(Graph_Rect_Angle);																/* Screen To Client */
	ScreenToClient(Graph_Rect_Encoder);

	pGraph_Angle = new COScopeCtrl(1);																/* 각도에 대한 그래프 설정 */
	pGraph_Angle->Create(WS_VISIBLE | WS_CHILD, Graph_Rect_Angle, this, IDC_STATIC_GRAPH_ENGLE);
	pGraph_Angle->SetRanges(-27.0, 27.0);
	pGraph_Angle->autofitYscale = true;
	pGraph_Angle->SetYUnits(_T("Value"));
	pGraph_Angle->SetXUnits(_T("Time"));
	pGraph_Angle->SetLegendLabel(_T("Angle"), 0);
	pGraph_Angle->SetPlotColor(RGB(255,0,0), 0);
	pGraph_Angle->InvalidateCtrl();

	pGraph_Encoder = new COScopeCtrl(2);															/* 엔코더에 대한 그래프 설정 */
	pGraph_Encoder->Create(WS_VISIBLE | WS_CHILD, Graph_Rect_Encoder, this, IDC_STATIC_GRAPH_ENGLE);
	pGraph_Encoder->SetRanges(-2000.0, 2000.0);
	pGraph_Encoder->autofitYscale = true;
	pGraph_Encoder->SetYUnits(_T("Value"));
	pGraph_Encoder->SetXUnits(_T("Time"));
	pGraph_Encoder->SetLegendLabel(_T("Step Pulse(Left)"), 0);
	pGraph_Encoder->SetLegendLabel(_T("Step Pulse(Right)"), 1);
	pGraph_Encoder->SetPlotColor(RGB(0,0,255), 0);
	pGraph_Encoder->SetPlotColor(RGB(0,255,0), 1);
	pGraph_Encoder->InvalidateCtrl();
}


void CCommand_CenterDlg::DriveCommand(char dir)
{
	memset(&pc2mcu, 0, sizeof(pc2mcu));
	pc2mcu.drive.dir = dir;
	UpdateData(TRUE), pc2mcu.drive.speed = (short)((int)((long)m_speed_cmd));
	CreateCheckSum_PC2MCU();
	pListenSocket->BroadCast(&pc2mcu, sizeof(pc2mcu));
}


void CCommand_CenterDlg::CreateCheckSum_PC2MCU(void)
{
	char* ptr = (char*)(&pc2mcu);
	int temp = (int)NULL;
	for(int i=1; i<sizeof(pc2mcu); i++){
		temp += ptr[i];								/* 모두 더하기 */
		temp &= 0x000000FF;							/* 8비트만 마스킹 */
	}
	pc2mcu.checksum = (char)temp;
}