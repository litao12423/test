// PageStb1.cpp : implementation file
//

#include "stdafx.h"
#include "A19_VC01.h"
#include "PageStb1.h"
#include "CSerialPort.h"

#include "config.h"
#include "A19_VC01Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageStb1 dialog

extern CSerialPort AvPort[2];    //11,13
extern CSerialPort VolPort[3];   //27-29
extern CSerialPort StbPort[8];  //3-10
extern CSerialPort UutPort[8];  //19-26
extern CSerialPort LnbM3Port[2]; //30,31
extern CSerialPort SwitchM3Port[2]; //32, 33

extern int VolIndex[];
extern int Index22k[];

CPageStb1::CPageStb1(CWnd* pParent /*=NULL*/)
	: CDialog(CPageStb1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageStb1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPageStb1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageStb1)
	DDX_Control(pDX, IDC_LIST2, m_listResult);
	DDX_Control(pDX, IDC_LIST1, m_listStb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageStb1, CDialog)
	//{{AFX_MSG_MAP(CPageStb1)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMM_RXCHAR, OnComm)
END_MESSAGE_MAP()

LONG CPageStb1::OnComm(WPARAM ch, LPARAM port) 
{	
	static cnt_ir = 0;

	int StbFloor = 1;
	CString strSend = "";
	CString floor;
	int TimerId = 0;

	//m_editSerial01 += (char)ch;	
	m_stbSerial += (char)ch;

	//m_listStb.InsertString(0, "test!");
	if (ch == '\n')
	{
		CA19_VC01App * pApp = (CA19_VC01App*)AfxGetApp();
		CA19_VC01Dlg * pDlg = (CA19_VC01Dlg*)pApp->m_pMainWnd;

		floor.Format("%d", StbFloor);

		if (m_stbSerial.Find("A11:1A000000") != -1)  //
		{
			strSend = "A11:1A000000";   //发送握手指令
			StbPort[StbFloor-1].WriteToPort(strSend);
			strSend = ""; 		
		}
		else if (m_stbSerial.Find("A11:1A100000") != -1)  //
		{
			m_listResult.InsertString(0, "A11:1A100000 ShankHands OK"); 
			strSend = "H" + floor + "-OK\r\n";                     //机顶盒握手成功，可以测试

			char sendchar[3] = {0xaa, 0x5c, 0x5c};          //改变工装指示灯颜色
			UutPort[StbFloor-1].WriteToPort(sendchar, 3);
		}
		else if ((m_stbSerial.Find("A11:6O000000") != -1) && m_stbSerial.GetLength() < 15)  //LAN
		{
			m_listResult.InsertString(0, "A11:6O000000 LAN OK");  
			strSend= "T" + floor + "Lan-OK\r\n";

			TimerId = 100 + StbFloor*10 + 0;
			TestTimerFlag[StbFloor-1][0] = 0;
		}
		else if ((m_stbSerial.Find("A11:6O100000") != -1) && m_stbSerial.GetLength() < 15)  //LAN
		{
			m_listResult.InsertString(0, "A11:6O100000 LAN Fail");  
			strSend= "T" + floor + "Lan-NG\r\n";

			TimerId = 100 + StbFloor*10 + 0;
			TestTimerFlag[StbFloor-1][0] = 0;
		}
		else if ((m_stbSerial.Find("A11:7P100000") != -1) && m_stbSerial.GetLength() < 15)  //USB
		{
			m_listResult.InsertString(0, "A11:7P100000 USB OK");  
			strSend= "T" + floor + "Usb-OK\r\n";

			TimerId = 100 + StbFloor*10 + 1;
			TestTimerFlag[StbFloor-1][1] = 0;
		}
		else if ((m_stbSerial.Find("A11:7P000000") != -1) && m_stbSerial.GetLength() < 15)  //USB
		{
			m_listResult.InsertString(0, "A11:7P000000 USB Fail");  
			strSend= "T" + floor + "Usb-NG\r\n";

			TimerId = 100 + StbFloor*10 + 1;
			TestTimerFlag[StbFloor-1][1] = 0;
		}
		else if (((m_stbSerial.Find("A11:8Q300000") != -1) || (m_stbSerial.Find("A11:8Q100000") != -1) || (m_stbSerial.Find("A11:8Q200000") != -1)) 
				&& m_stbSerial.GetLength() < 15)  //KEY
		{
			m_listResult.InsertString(0, "A11:8Q300000 KEY OK");  
			strSend= "T" + floor + "Key-OK\r\n";

			TimerId = 100 + StbFloor*10 + 4;
			TestTimerFlag[StbFloor-1][4] = 0;
		}
		else if ((m_stbSerial.Find("A11:8Q000000") != -1) && m_stbSerial.GetLength() < 15)  //KEY
		{
			m_listResult.InsertString(0, "A11:8Q000000 KEY Fail");  
			strSend= "T" + floor + "Key-NG\r\n";

			TimerId = 100 + StbFloor*10 + 4;
			TestTimerFlag[StbFloor-1][4] = 0;
		}
		else if ((m_stbSerial.Find("A11:8SCE0000") != -1) && m_stbSerial.GetLength() < 15)  //IR
		{
			m_listResult.InsertString(0, "A11:8SCE0000 IR OK");  
			strSend= "T" + floor + "IR-OK\r\n";
			cnt_ir = 0;

			TimerId = 100 + StbFloor*10 + 5;
			TestTimerFlag[StbFloor-1][5] = 0;
		}
		else if ((m_stbSerial.Find("A11:8S000000") != -1) && m_stbSerial.GetLength() < 15)  //IR
		{
			m_listResult.InsertString(0, "A11:8S000000 IR Fail");  
			strSend= "T" + floor + "IR-NG\r\n";

			TimerId = 100 + StbFloor*10 + 5;
			TestTimerFlag[StbFloor-1][5] = 0;
		}
		else if ((m_stbSerial.Find("A11:9T000000") != -1) && m_stbSerial.GetLength() < 15)  //CA
		{
			m_listResult.InsertString(0, "A11:9T000000 CA OK");  
			strSend= "T" + floor + "Ca-OK\r\n";

			TimerId = 100 + StbFloor*10 + 2;
			TestTimerFlag[StbFloor-1][2] = 0;
		}
		else if ((m_stbSerial.Find("A11:9T100000") != -1) && m_stbSerial.GetLength() < 15)  //CA
		{
			m_listResult.InsertString(0, "A11:9T100000 CA Fail");  
			strSend= "T" + floor + "Ca-NG\r\n";

			TimerId = 100 + StbFloor*10 + 2;
			TestTimerFlag[StbFloor-1][2] = 0;
		}
		else if ((m_stbSerial.Find("A11::U000000") != -1) && m_stbSerial.GetLength() < 15)  //TUNER
		{
			m_listResult.InsertString(0, "A11::U000000 TUNER OK");  
			strSend= "T" + floor + "Tuner-OK\r\n";

			TimerId = 100 + StbFloor*10 + 3;
			TestTimerFlag[StbFloor-1][3] = 0;
		}
		else if ((m_stbSerial.Find("A11::U100000") != -1) && m_stbSerial.GetLength() < 15)  //TUNER
		{
			m_listResult.InsertString(0, "A11::U100000 TUNER Fail"); 
			strSend= "T" + floor + "Tuner-NG\r\n";

			TimerId = 100 + StbFloor*10 + 3;
			TestTimerFlag[StbFloor-1][3] = 0;
		}
		else if ((m_stbSerial.Find("A11:5N000000") != -1) && m_stbSerial.GetLength() < 15)  //LNB
		{
			m_listResult.InsertString(0, "A11:5N000000 LNB"); 
			strSend = "C" + floor + CSItem[3+VolIndex[(StbFloor+1)%2]] + "-" + "OK" + "\r\n";    //LNB交互成功
			Clnbstate[StbFloor-1][Clnbindex[StbFloor-1]] = 0;
		}

		if ((!strSend.IsEmpty()) && (pDlg->m_clientSocket.m_bConnected))  //发送测试结果
		{
			pDlg->m_clientSocket.Send(strSend, strSend.GetLength(), 0);
			strSend = "";
		}
		
		m_listStb.InsertString(0, m_stbSerial);
		m_stbSerial = "";
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPageStb1 message handlers
