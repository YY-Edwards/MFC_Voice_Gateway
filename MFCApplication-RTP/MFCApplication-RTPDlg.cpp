
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

	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);//init socket

	pThis = this;
	channel1RTP = new MyRTP;
	mastergate = new JProtocol;
	//myreceiver = new MyRTPReceiver;


}

CMFCApplicationRTPDlg::~CMFCApplicationRTPDlg()
{
	WSACleanup();
	if (channel1RTP->IsActive())channel1RTP->BYEDestroy(RTPTime(0, 10), "Time's up", 9);
	if (channel1RTP != NULL)delete channel1RTP;
	mastergate->CloseMater();
	if (mastergate != NULL)delete mastergate;

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

	channel1RTP->Rtp_Init(55500, 57400, 1973);
	uint8_t silencebuffer[320];
	for (int i = 0; i < 320; i++)
		silencebuffer[i] = 128;

	for (int j = 0; j < 50; j++)
	{

		channel1RTP->SendRTPPayloadData(silencebuffer, 320);
		RTPTime::Wait(RTPTime(1, 20));
		TRACE(_T(" Wait okay!\r\n"));

	}
	
	channel1RTP->Set_Exit_Flag();
	//channe21RTP->Rtp_Init(56500, 58400, 2012);

	TRACE(_T(" channel1RTP->Rtp_Init okay!\r\n"));
	
	// TODO:  在此添加控件通知处理程序代码
}



void CMFCApplicationRTPDlg::MasterOnData(int command, ResponeData data)
{

	if (pThis == NULL)exit(-1);
	pThis->MasterOnDataFunc(command, data);

}
void CMFCApplicationRTPDlg::MasterOnDataFunc(int command, ResponeData data)
{
	PROTOCOL_Names pro_name = (PROTOCOL_Names)command;
	switch (pro_name)
	{
	case CONNECT:
			mastergate->ConnectReply("success", "fine!");
		break;


	case LISTENING:
			mastergate->ConfigReply();
		break;

	case QUERY:
			mastergate->QueryReply();
		break;


	case CALLREQUEST:
			mastergate->CallRequestReply();
		break;

	case CALLRELEASE:
			mastergate->CallRequestReply();
		break;
	default:

		break;


	}

	TRACE(_T("MasterOnDataFunc finished\n"));

}


void CMFCApplicationRTPDlg::OnBnClickedButton_TCPInit()
{
	
	mastergate->SetCallBackFunc(MasterOnData);//设置回调函数
	mastergate->Start();//启动master
	
	CWnd *pWnd1 = GetDlgItem(IDC_BUTTON2);
	pWnd1->EnableWindow(FALSE);
	
	
	// TODO:  在此添加控件通知处理程序代码
}
