
// MFCApplication-RTPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCApplication-RTP.h"
#include "MFCApplication-RTPDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCApplicationRTPDlg *CMFCApplicationRTPDlg::pThis = NULL;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMFCApplicationRTPDlg �Ի���



CMFCApplicationRTPDlg::CMFCApplicationRTPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCApplicationRTPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//WSADATA dat;
	//WSAStartup(MAKEWORD(2, 2), &dat);//init socket

	pThis = this;
	fp = NULL;;
	channel1RTP = NULL;
	channel2RTP = NULL;
	mastergate = new JProtocol;


}

CMFCApplicationRTPDlg::~CMFCApplicationRTPDlg()
{
	//WSACleanup();

	if (channel1RTP != NULL)
	{
		delete channel1RTP;
		channel1RTP = NULL;
		if (fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
	}
	if (channel2RTP != NULL)
	{
		delete channel2RTP;
		channel2RTP = NULL;
	}
	if (mastergate != NULL)
	{
		delete mastergate;
		mastergate = NULL;
	}

}

void CMFCApplicationRTPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplicationRTPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplicationRTPDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplicationRTPDlg::OnBnClickedButton_TCPInit)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplicationRTPDlg::OnBnClickedSendStart)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplicationRTPDlg::OnBnClickedSendEnd)
END_MESSAGE_MAP()


// CMFCApplicationRTPDlg ��Ϣ�������

BOOL CMFCApplicationRTPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCApplicationRTPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCApplicationRTPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCApplicationRTPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplicationRTPDlg::OnBnClickedButton1()
{
	CWnd *pWnd1 = GetDlgItem(IDC_BUTTON1);
	pWnd1->EnableWindow(FALSE);

	PROTOCOL_Fixed_Header_t header;
	int len = 0;
	len = sizeof(header);

	header.identifier = "statusininfsffsfsfsfsvvvff";
	len = sizeof(header);

	header.identifier = "NULL";
	len = sizeof(header);



	//channel1RTP->RtpParamsInit(55500, 57400, 1973);
	//uint8_t silencebuffer[320];
	//for (int i = 0; i < 320; i++)
	//	silencebuffer[i] = 128;

	//for (int j = 0; j < 50; j++)
	//{

	//	channel1RTP->SendRTPPayloadData(silencebuffer, 320);
	//	RTPTime::Wait(RTPTime(1, 20));
	//	TRACE(_T(" Wait okay!\r\n"));

	//}
	//
	//channel1RTP->SetThreadExitFlag();
	//channe21RTP->RtpParamsInit(56500, 58400, 2012);

	TRACE(_T(" channel1RTP->RtpParamsInit okay!\r\n"));
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CMFCApplicationRTPDlg::OnBnClickedButton_TCPInit()
{
	
	mastergate->SetCallBackFunc(MasterOnData);//���ûص�����
	mastergate->Start();//����master
	
	CWnd *pWnd1 = GetDlgItem(IDC_BUTTON2);
	pWnd1->EnableWindow(FALSE);
	
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CMFCApplicationRTPDlg::OnBnClickedSendStart()
{
	int src = 0xffff;
	int dst = 0xabcd;
	std::string channel = "channel0";
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (mastergate!= NULL)
	{
		if (mastergate->IsMaterInitComplete())mastergate->CallStartNotify(0, src, dst, channel);

	}



}

void CMFCApplicationRTPDlg::OnBnClickedSendEnd()
{
	int src = 0xffff;
	int dst = 0xabcd;
	std::string channel = "channel0";
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	if (mastergate != NULL)
	{
		if (mastergate->IsMaterInitComplete())mastergate->CallEndNotify(0, src, dst, channel);

	}

}



void CMFCApplicationRTPDlg::RTPChannel1OnData(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->RTPChannel1OnDataFunc(data);

}
void CMFCApplicationRTPDlg::RTPChannel1OnDataFunc(ResponeRTPData data)
{
	static int count = 0;
	count++;
	fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);
	
}

void CMFCApplicationRTPDlg::RTPChannel2OnData(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->RTPChannel2OnDataFunc(data);

}
void CMFCApplicationRTPDlg::RTPChannel2OnDataFunc(ResponeRTPData data)
{
	static int count = 0;
	count++;
	TRACE(_T("RTPChannel-2: Receive %d packet of voice data\n"), count);

}


void CMFCApplicationRTPDlg::MasterOnData(int command, ResponeData data)
{

	if (pThis == NULL)exit(-1);
	pThis->MasterOnDataFunc(command, data);

}
void CMFCApplicationRTPDlg::MasterOnDataFunc(int command, ResponeData data)
{
	std::string c_status = "success";
	//char *sendVoiceBuff = {"abcdefghiojklmnopqrstuvwxyz"};
	char sendVoiceBuff[50] = "abcdefghiojklmnopqrstuvwxyz";

	PROTOCOL_Names pro_name = (PROTOCOL_Names)command;
	switch (pro_name)
	{
	case CONNECT:
		mastergate->ConnectReply(data.socket_fd, "success", "fine!");
		if (1)/*PTT Notice Enable*/
		{

		}
		break;


	case LISTENING:
		if (data.channel1_group_id != 0)
		{
			if (channel1RTP == NULL){
				channel1RTP = new MyRTP;
				channel1RTP->RtpParamsInit(CHANNEL1RTPBASEPORT, CHANNEL1RTPDESTPORT, CHANNEL1RTPSSRC);//�Ƿ���ʽ���ã�
				channel1RTP->SetCallBackFunc(RTPChannel1OnData);

			}
			TRACE(("set channel1 is:%d\n"), data.channel1_group_id);

			fp = fopen("E:\\CloudMusic\\master_recv_voice.pcm", "wb+");
			if (fp == NULL){
				printf("fp  fail\n");
			}
			fseek(fp, 0, SEEK_SET);

		}

		if (data.channel2_group_id != 0)
		{
			if (channel2RTP == NULL){
				channel2RTP = new MyRTP;
				channel2RTP->RtpParamsInit(CHANNEL2RTPBASEPORT, CHANNEL2RTPDESTPORT, CHANNEL2RTPSSRC);
				channel2RTP->SetCallBackFunc(RTPChannel2OnData);
			}
			TRACE(("set channel2 is:%d\n"), data.channel2_group_id);
		}
		if (data.channel1_group_id == 0 && data.channel2_group_id == 0)
		{
			TRACE(_T("No channel is set\n"));
		}
		mastergate->ConfigReply(data.socket_fd, data.channel1_group_id, data.channel2_group_id);
		break;

	case QUERY:
		mastergate->QueryReply(data.socket_fd, data.channel1_group_id, data.channel2_group_id);
		break;


	case CALLREQUEST:
		mastergate->CallRequestReply(data.socket_fd, "fail", "Unconnect");
		if (c_status == "success")
		{
			TRACE(_T("Start to send RTP Voice\n"));
			mastergate->CallStartNotify(data.socket_fd, data.src_id, data.dst_id, data.channel_id);

			if ((data.channel_id == "channel1") && channel1RTP != NULL)
				channel1RTP->SendRTPPayloadData(sendVoiceBuff, 24);

			if ((data.channel_id == "channel2") && channel2RTP != NULL)
				channel2RTP->SendRTPPayloadData(sendVoiceBuff, 24);

		}
		break;

	case CALLRELEASE:
		mastergate->CallReleaseReply(data.socket_fd, "fail", "Unconnect");
		if (c_status == "success")
		{
			TRACE(_T("Stop to send RTP Voice\n"));
			mastergate->CallEndNotify(data.socket_fd, data.src_id, data.dst_id, data.channel_id);
		}
		break;
	default:

		break;


	}

	TRACE(_T("MasterOnDataFunc finished\n"));

}