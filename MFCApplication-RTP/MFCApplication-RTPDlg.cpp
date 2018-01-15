
// MFCApplication-RTPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication-RTP.h"
#include "MFCApplication-RTPDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCApplicationRTPDlg *CMFCApplicationRTPDlg::pThis = NULL;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMFCApplicationRTPDlg 对话框



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


// CMFCApplicationRTPDlg 消息处理程序

BOOL CMFCApplicationRTPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplicationRTPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	
	// TODO:  在此添加控件通知处理程序代码
}

void CMFCApplicationRTPDlg::OnBnClickedButton_TCPInit()
{
	
	mastergate->SetCallBackFunc(MasterOnData);//设置回调函数
	mastergate->Start();//启动master
	
	CWnd *pWnd1 = GetDlgItem(IDC_BUTTON2);
	pWnd1->EnableWindow(FALSE);
	
	
	// TODO:  在此添加控件通知处理程序代码
}

void CMFCApplicationRTPDlg::OnBnClickedSendStart()
{
	int src = 0xffff;
	int dst = 0xabcd;
	std::string channel = "channel0";
	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  在此添加控件通知处理程序代码

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
				channel1RTP->RtpParamsInit(CHANNEL1RTPBASEPORT, CHANNEL1RTPDESTPORT, CHANNEL1RTPSSRC);//是否隐式设置？
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