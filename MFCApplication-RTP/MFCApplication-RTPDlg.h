
// MFCApplication-RTPDlg.h : ͷ�ļ�
//

#pragma once

#include "myrtp.h"
#include "myprotocol.h"


// CMFCApplicationRTPDlg �Ի���
class CMFCApplicationRTPDlg : public CDialogEx
{
// ����
public:
	CMFCApplicationRTPDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CMFCApplicationRTPDlg();	// ��׼��������

// �Ի�������
	enum { IDD = IDD_MFCAPPLICATIONRTP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton1();
	MyRTP *channel1RTP;
	MyRTP *channel2RTP;
	JProtocol *mastergate;

	static CMFCApplicationRTPDlg *pThis;
	//protocol callbackfunc
	static void  MasterOnData(int command, ResponeData data);
	void  MasterOnDataFunc(int command, ResponeData data);

	//rtp callbackfunc
	static void  RTPChannel1OnData(ResponeRTPData data);
	void  RTPChannel1OnDataFunc(ResponeRTPData data);

	static void  RTPChannel2OnData(ResponeRTPData data);
	void  RTPChannel2OnDataFunc(ResponeRTPData data);
	
	FILE *fp;

	afx_msg void OnBnClickedButton_TCPInit();
	afx_msg void OnBnClickedSendStart();
	afx_msg void OnBnClickedSendEnd();
};
