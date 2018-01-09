
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
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	
	mastergate->SetCallBackFunc(MasterOnData);//���ûص�����
	mastergate->Start();//����master
	
	CWnd *pWnd1 = GetDlgItem(IDC_BUTTON2);
	pWnd1->EnableWindow(FALSE);
	
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
