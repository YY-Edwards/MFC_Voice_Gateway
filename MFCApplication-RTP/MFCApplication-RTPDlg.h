
// MFCApplication-RTPDlg.h : 头文件
//

#pragma once

#include "myrtp.h"
#include "myprotocol.h"


// CMFCApplicationRTPDlg 对话框
class CMFCApplicationRTPDlg : public CDialogEx
{
// 构造
public:
	CMFCApplicationRTPDlg(CWnd* pParent = NULL);	// 标准构造函数

	~CMFCApplicationRTPDlg();	// 标准析构函数

// 对话框数据
	enum { IDD = IDD_MFCAPPLICATIONRTP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	

	afx_msg void OnBnClickedButton_TCPInit();
};
