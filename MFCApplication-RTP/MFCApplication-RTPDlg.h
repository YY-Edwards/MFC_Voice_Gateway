
// MFCApplication-RTPDlg.h : ͷ�ļ�
//

#pragma once

#include "myrtp.h"


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

};
