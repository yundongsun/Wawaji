// EnterChannelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraWawajiDemo.h"
#include "EnterChannelDlg.h"
#include "afxdialogex.h"


// CEnterChannelDlg 对话框

IMPLEMENT_DYNAMIC(CEnterChannelDlg, CDialogEx)

CEnterChannelDlg::CEnterChannelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEnterChannelDlg::IDD, pParent)
{
    
}

CEnterChannelDlg::~CEnterChannelDlg()
{
}

void CEnterChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_APPID, m_ctrAppId);
	DDX_Control(pDX, IDC_EDCHNAME_CHANNEL, m_ctrChannel);
	DDX_Control(pDX, IDC_BTNTEST_CHANNEL, m_btnTest);
	DDX_Control(pDX, IDC_BTNJOIN_CHANNEL, m_btnJoin);
	DDX_Control(pDX, IDC_BTNSET_CHANNEL, m_btnSetup);
	DDX_Control(pDX, IDC_EDCHPSWD_CHANNEL, m_ctrPassword);
	DDX_Control(pDX, IDC_EDIT_UID, m_ctrUid);
}


BEGIN_MESSAGE_MAP(CEnterChannelDlg, CDialogEx)
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTNTEST_CHANNEL, &CEnterChannelDlg::OnBnClickedBtntestChannel)
	ON_BN_CLICKED(IDC_BTNJOIN_CHANNEL, &CEnterChannelDlg::OnBnClickedBtnjoinChannel)
	ON_BN_CLICKED(IDC_BTNSET_CHANNEL, &CEnterChannelDlg::OnBnClickedBtnsetChannel)
	ON_CBN_SELCHANGE(IDC_CMBROLE_CHANNEL, &CEnterChannelDlg::OnCbnSelchangeCmbRole)

END_MESSAGE_MAP()


// CEnterChannelDlg 消息处理程序
BOOL CEnterChannelDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN){
		switch (pMsg->wParam){
		case VK_ESCAPE:
			return FALSE;
		case VK_RETURN:
			OnBnClickedBtnjoinChannel();
			return FALSE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CEnterChannelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ftHead.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_ftDesc.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_ftBtn.CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_penFrame.CreatePen(PS_SOLID, 1, RGB(0xD8, 0xD8, 0xD8));

	m_dlgDevice.Create(CDeviceDlg::IDD, this);
	m_dlgDevice.EnableDeviceTest(TRUE);

	SetBackgroundColor(RGB(0xFF, 0xFF, 0xFF));
	InitCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CEnterChannelDlg::InitCtrls()
{
	CRect ClientRect;

	GetClientRect(&ClientRect);

	m_ctrAppId.MoveWindow(160, 33, 240, 22, TRUE);
	m_ctrAppId.SetFont(&m_ftDesc);
	m_ctrAppId.SetCaretPos(CPoint(12, 148));
	m_ctrAppId.ShowCaret();
	m_ctrAppId.SetTip(_T("324f0da1e2284832a44fee5fcbec44c1"));
	m_ctrAppId.SetFocus();
	
	m_ctrChannel.MoveWindow(200, 82, 200, 22, TRUE);
    m_ctrChannel.SetFont(&m_ftDesc);
	m_ctrChannel.SetCaretPos(CPoint(12, 148));
	m_ctrChannel.ShowCaret();
	m_ctrChannel.SetTip(_T("wawaji_demo")); 
	m_ctrChannel.SetFocus();
    
//     m_ctrPassword.MoveWindow(90, 131, 120, 22, TRUE);
// 	m_ctrPassword.SetFont(&m_ftDesc);
// 	m_ctrPassword.SetTip(LANG_STR("IDS_CHN_ROOMPASSWORD"));
// 	m_ctrPassword.SetFocus();

	m_ctrUid.MoveWindow(120, 131, 90, 22, TRUE);
	m_ctrUid.SetFont(&m_ftDesc);
	m_ctrUid.SetTip(_T("1"));
	m_ctrUid.SetFocus();

	m_ctrRole.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE, CRect(ClientRect.Width() / 2 + 1, 168, 180, 32), this, IDC_CMBROLE_CHANNEL);
	m_ctrRole.MoveWindow(280, 127, 130, 22, TRUE);
	m_ctrRole.SetFont(&m_ftDesc);
	m_ctrRole.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_ctrRole.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));
	m_ctrRole.InsertString(0, LANG_STR("IDS_CHN_CASTER"));
	m_ctrRole.InsertString(1, LANG_STR("IDS_CHN_AUDIENCE"));
	m_ctrRole.SetCurSel(0);

	m_ctrRole.SetCurSel(0);
   
	m_btnJoin.MoveWindow(70, 310, 360, 36, TRUE);
	m_btnTest.MoveWindow(70, 355, 108, 36, TRUE);
	m_btnSetup.MoveWindow(190, 355, 240, 36, TRUE);

	m_btnJoin.SetBackColor(RGB(0x00, 0xA0, 0xE9), RGB(0x05, 0x78, 0xAA), RGB(0x05, 0x78, 0xAA), RGB(0xE6, 0xE6, 0xE6));
	m_btnJoin.SetFont(&m_ftBtn);
	m_btnJoin.SetTextColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xCC, 0xCC, 0xCC));
	m_btnJoin.SetWindowText(LANG_STR("IDS_CHN_BTJOIN"));

	m_btnTest.SetBorderColor(RGB(0xD8, 0xD8, 0xD8), RGB(0x00, 0xA0, 0xE9), RGB(0x00, 0xA0, 0xE9), RGB(0xCC, 0xCC, 0xCC));
	m_btnTest.SetBackColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));
	m_btnTest.SetFont(&m_ftBtn);
	m_btnTest.SetTextColor(RGB(0x55, 0x58, 0x5A), RGB(0x00, 0xA0, 0xE9), RGB(0x00, 0xA0, 0xE9), RGB(0xCC, 0xCC, 0xCC));
	m_btnTest.SetWindowText(LANG_STR("IDS_CHN_BTTEST"));

	m_btnSetup.SetBorderColor(RGB(0xD8, 0xD8, 0xD8), RGB(0x00, 0xA0, 0xE9), RGB(0x00, 0xA0, 0xE9), RGB(0xCC, 0xCC, 0xCC));
	m_btnSetup.SetBackColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));
	m_btnSetup.SetFont(&m_ftBtn);
	m_btnSetup.SetTextColor(RGB(0x55, 0x58, 0x5A), RGB(0x00, 0xA0, 0xE9), RGB(0x00, 0xA0, 0xE9), RGB(0xCC, 0xCC, 0xCC));
	m_btnSetup.SetWindowText(_T("1920*1080,15fps, 3mbps"));

	CMFCButton::EnableWindowsTheming(FALSE);
}

void CEnterChannelDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawClient(&dc);
}


void CEnterChannelDlg::DrawClient(CDC *lpDC)
{
	CRect	rcText;
	CRect	rcClient;
	LPCTSTR lpString = NULL;

	GetClientRect(&rcClient);

    CFont* defFont = lpDC->SelectObject(&m_ftDesc);
	lpDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
	lpDC->SetTextColor(RGB(0x44, 0x45, 0x46));

	lpDC->SelectObject(&m_penFrame);
	rcText.SetRect(rcClient.Width() / 2 - 180, 25, rcClient.Width() / 2 + 170, 57);
	lpDC->RoundRect(&rcText, CPoint(32, 32));
	lpDC->TextOut(rcClient.Width() / 2 - 175 ,32, _T("APPID"));

	rcText.OffsetRect(0, 49);
	lpDC->RoundRect(&rcText, CPoint(32, 32));
	lpDC->TextOut(rcClient.Width() / 2 - 175, 81, _T("ChannelName"));

	rcText.OffsetRect(0, 49);
	lpDC->RoundRect(&rcText, CPoint(32, 32));
	lpDC->TextOutW(rcClient.Width() /2 -175,130,_T("UID"));

	lpString = LANG_STR("IDS_CHN_ROLETITLE");
	lpDC->SetTextColor(RGB(0xD8, 0xD8, 0xD8));
	lpDC->TextOut(220, 80, lpString, _tcslen(lpString));

	lpDC->SelectObject(defFont);

	// Done with the font.  Delete the font object.
	//	font.DeleteObject();
}

void CEnterChannelDlg::OnBnClickedBtntestChannel()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strAppId;
	m_ctrAppId.GetWindowText(strAppId);	
	if (strAppId.IsEmpty() || strAppId == _T("APPID"))
	{
		MessageBox(_T("前先填写APPID."), _T("提示"), MB_ICONINFORMATION);
		return;
	}
	CAgoraObject* m_lpAgoraObject = CAgoraObject::GetAgoraObject(strAppId);
	m_lpAgoraObject->SetLogFilePath(NULL);
	m_lpAgoraObject->SetMsgHandlerWnd(GetSafeHwnd());
	CAgoraObject::GetEngine()->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lpAgoraObject->EnableVideo(TRUE);
	m_lpAgoraObject->SetClientRole(CLIENT_ROLE_BROADCASTER);
	CAgoraObject::GetAgoraObject()->EnableLastmileTest(TRUE);

	m_dlgDevice.ShowWindow(SW_SHOW);
	m_dlgDevice.CenterWindow();
}


void CEnterChannelDlg::OnBnClickedBtnjoinChannel()
{
	// TODO:  在此添加控件通知处理程序代码
	CString     strKey;
    CString     strChannelName;
    CString     strInfo;
    CString     strOperation;
    BOOL        bFound = FALSE;
    BOOL        bSuccess = FALSE;

	CString strAppId;
	m_ctrAppId.GetWindowText(strAppId);
	if (strAppId.IsEmpty() || strAppId == _T("APPID"))
	{
		MessageBox(_T("前先填写APPID."), _T("提示"), MB_ICONINFORMATION);
		return;
	}
	CAgoraObject* m_lpAgoraObject = CAgoraObject::GetAgoraObject(strAppId);
	IRtcEngine *pRtcEngine = CAgoraObject::GetEngine();
	
	RtcEngineParameters rep(pRtcEngine);
	rep.enableWebSdkInteroperability(true);
	m_lpAgoraObject->SetLogFilePath(NULL);
	m_lpAgoraObject->SetMsgHandlerWnd(GetSafeHwnd());
	CAgoraObject::GetEngine()->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lpAgoraObject->EnableVideo(TRUE);
	m_lpAgoraObject->SetClientRole(CLIENT_ROLE_BROADCASTER);
	m_lpAgoraObject->SetSelfUID(0);
	m_lpAgoraObject->SetAppCert(_T("164aa13965394ffbb5ebeb43c4c7ed5c"));
	m_lpAgoraObject->EnableLocalMirrorImage(FALSE);

	CAgoraObject::GetAgoraObject()->EnableLastmileTest(TRUE);

    m_ctrChannel.GetWindowText(strChannelName);
    m_ctrPassword.GetWindowText(strKey);

	m_dlgDevice.videoExternCapture(TRUE);

	GetParent()->SendMessage(WM_JOINCHANNEL, 0, 0);
}


void CEnterChannelDlg::OnBnClickedBtnsetChannel()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strAppId;
	m_ctrAppId.GetWindowText(strAppId);
	if (strAppId.IsEmpty() || strAppId == _T("APPID"))
	{
		MessageBox(_T("前先填写APPID."), _T("提示"), MB_ICONINFORMATION);
		return;
	}
	CAgoraObject* m_lpAgoraObject  = CAgoraObject::GetAgoraObject(strAppId);
	m_lpAgoraObject->SetLogFilePath(NULL);
	m_lpAgoraObject->SetMsgHandlerWnd(GetSafeHwnd());
	CAgoraObject::GetEngine()->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lpAgoraObject->EnableVideo(TRUE);
	m_lpAgoraObject->SetClientRole(CLIENT_ROLE_BROADCASTER);
	CAgoraObject::GetAgoraObject()->EnableLastmileTest(TRUE);

	GetParent()->SendMessage(WM_GONEXT, 0, 0);
}

void CEnterChannelDlg::OnCbnSelchangeCmbRole()
{
	int nSel = m_ctrRole.GetCurSel();

	if (nSel == 0)
		CAgoraObject::GetAgoraObject()->SetClientRole(CLIENT_ROLE_BROADCASTER);
	else
		CAgoraObject::GetAgoraObject()->SetClientRole(CLIENT_ROLE_AUDIENCE);
}

CString CEnterChannelDlg::GetChannelName()
{
	CString strChannelName;

	m_ctrChannel.GetWindowText(strChannelName);

	return strChannelName;
}

void CEnterChannelDlg::SetVideoString(LPCTSTR lpVideoString)
{
	m_btnSetup.SetWindowText(lpVideoString);
}