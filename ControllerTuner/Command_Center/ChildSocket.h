#pragma once
#include "listensocket.h"
#include "string.h"
#include <queue>

using namespace std;

// CChildSocket 명령 대상입니다.
class CCommand_CenterDlg;

//#define CALLBACK 0x80

typedef struct Flag
{
	bool button_clicked;
	bool receive;
}Flag;

typedef unsigned char Byte;

typedef unsigned char Checksum;

typedef struct ENCODER
{
	short left;
	short right;
}ENCODER;

typedef struct MONITORING
{
	ENCODER encoder;
	double angle;
}MONITORING;

typedef struct MCU2PC
{
	MONITORING monitoring;
	Byte command;
}MCU2PC;

typedef struct Gain
{
	double pPosture;
	double iPosture;
	double dPosture;
	double pCentroid;
	double iCentroid;
	double dCentroid;
	double pPosition;
	double iPosition;
}Gain;

typedef struct Drive
{
	char dir;
	short speed;
}Drive;

typedef struct PC2MCU
{
	Checksum checksum;
	Drive drive;
	Gain gain;
}PC2MCU;


class CChildSocket : public CSocket
{
public:
	CChildSocket();
	virtual ~CChildSocket();
	void SetListenSocket(CAsyncSocket* pSocket);
	CAsyncSocket* m_pListenSocket;
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	CCommand_CenterDlg* pDlg;
	MCU2PC mcu2pc;
	queue<MCU2PC> q;
	void Append2Graph(void);
	void delay_ms(clock_t n);
};


