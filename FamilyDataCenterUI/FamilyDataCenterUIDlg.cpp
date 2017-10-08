
// FamilyDataCenterUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "FamilyDataCenterUI/FamilyDataCenterUI.h"
#include "FamilyDataCenterUI/FamilyDataCenterUIDlg.h"
#include "FamilyDataCenterUI/FamilyDataController.h"
#include "FamilyDataCenterUI/FamilyDataModle.h"
#include "FamilyDataCenterUI/Config.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#undef max // 因为微软这个二比在某些头文件定义了max宏
#undef min // 因为微软这个二比在某些头文件定义了min宏
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#include "third_party/chromium/base/time/time.h"

namespace
{
    std::vector<std::wstring> family_columnlist = {
        L"主播",
        L"主播id",
        L"主播等级",
        L"开播次数",
        L"累计直播",
        L"直播时长",
        L"PC直播时长",
        L"手机直播时长",
        L"直播次数",
        L"直播有效天",
        L"直播间最高人气",
        L"星豆收入"
    };

    std::vector<std::wstring> singer_columnlist = {
        L"日期",
        L"开播次数",
        L"开播时长",
        L"有效直播次数",
        L"直播间最高人气",
        L"星豆收入",
        L"周期累计扣分"
    };

    bool OleDateTimeToBaseTime(const COleDateTime& oletime, base::Time* basetime)
    {
        if (!basetime)
            return false;

        SYSTEMTIME systemtime;
        if (!oletime.GetAsSystemTime(systemtime))
            return false;
        
        base::Time::Exploded exploded;
        exploded.year = systemtime.wYear;
        exploded.month = systemtime.wMonth;
        exploded.day_of_month = systemtime.wDay;
        exploded.day_of_week = systemtime.wDayOfWeek;
        exploded.hour = systemtime.wHour;
        exploded.minute = systemtime.wMinute;
        exploded.second = systemtime.wSecond;
        exploded.millisecond = systemtime.wMilliseconds;
        *basetime = base::Time::FromLocalExploded(exploded);
        return true;
    }

    bool SaveUserInfo(const std::wstring& username, const std::wstring& password,
        bool remember)
    {
        Config config;
        return config.Save(username, password, remember);
    }

    bool LoadUserInfo(std::wstring* username, std::wstring* password,bool* remeber)
    {
        Config config;
        bool result = config.GetUserName(username);
        config.GetPassword(password);
        *remeber = config.GetRemember();
        return result;
    }
}

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


// CFamilyDataCenterUIDlg 对话框



CFamilyDataCenterUIDlg::CFamilyDataCenterUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFamilyDataCenterUIDlg::IDD, pParent)
    , familyDataController_(nullptr)
    , familyDataModle_(nullptr)
    , m_oleDateTime_Begin(COleDateTime::GetCurrentTime())
    , m_oleDateTime_End(COleDateTime::GetCurrentTime())
    , m_username(_T(""))
    , m_password(_T(""))
    , index_(0)
    , m_remember(FALSE)
    , m_singer_id(0)
    , m_total_income(0)
    , m_total_hours(0)
    , m_new_count(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFamilyDataCenterUIDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SUMMARY_DATA, m_ListCtrl_SummaryData);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN, m_oleDateTime_Begin);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_oleDateTime_End);
    DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
    DDX_Control(pDX, IDC_LIST_MESSAGE, m_list_message);
    DDX_Check(pDX, IDC_CHECK_REMEMBER, m_remember);
    DDX_Text(pDX, IDC_EDIT_SINGER_ID, m_singer_id);
    DDX_Text(pDX, IDC_EDIT_TOTAL_INCOME, m_total_income);
    DDX_Text(pDX, IDC_EDIT_TOTAL_HOURS, m_total_hours);
    DDX_Text(pDX, IDC_EDIT_NEW_COUNT, m_new_count);
}

BEGIN_MESSAGE_MAP(CFamilyDataCenterUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_GET_FAMILY_DATA, &CFamilyDataCenterUIDlg::OnBnClickedGetFamilyData)
    ON_BN_CLICKED(IDC_BTN_EXPORT_TO_EXCEL, &CFamilyDataCenterUIDlg::OnBnClickedBtnExportToExcel)
    ON_BN_CLICKED(IDC_BTN_LOGIN, &CFamilyDataCenterUIDlg::OnBnClickedBtnLogin)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SUMMARY_DATA, &CFamilyDataCenterUIDlg::OnLvnGetdispinfoListSummaryData)
    ON_BN_CLICKED(IDC_BTN_GET_SINGER_DATA, &CFamilyDataCenterUIDlg::OnBnClickedBtnGetSingerData)
    ON_BN_CLICKED(IDC_BTN_GET_NEW_SINGER, &CFamilyDataCenterUIDlg::OnBnClickedBtnGetNewSinger)
END_MESSAGE_MAP()


// CFamilyDataCenterUIDlg 消息处理程序

BOOL CFamilyDataCenterUIDlg::OnInitDialog()
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
    DWORD dwStyle = m_ListCtrl_SummaryData.GetExtendedStyle();
    dwStyle |= LVS_REPORT;
    dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
    dwStyle |= LVS_OWNERDATA;
    dwStyle |= LVS_AUTOARRANGE;
    m_ListCtrl_SummaryData.SetExtendedStyle(dwStyle); //设置扩展风格

    familyDataController_.reset(new FamilyDataController);
    std::wstring display_msg;
    familyDataController_->LoadAuthority(&display_msg);
    if (!display_msg.empty())
    {
        SetWindowText(display_msg.c_str());
    }

    familyDataModle_.reset(new FamilyDataModle);

    std::wstring username;
    std::wstring password;
    bool remember = false;
    if (LoadUserInfo(&username, &password,&remember))
    {
        m_remember = static_cast<int>(remember);
        m_username = username.c_str();
        m_password = password.c_str();
        UpdateData(FALSE);
    }

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFamilyDataCenterUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFamilyDataCenterUIDlg::OnPaint()
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
HCURSOR CFamilyDataCenterUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFamilyDataCenterUIDlg::OnBnClickedGetFamilyData()
{
    UpdateData(TRUE);
    base::Time beginTime;
    base::Time endTime;
    OleDateTimeToBaseTime(m_oleDateTime_Begin, &beginTime);
    OleDateTimeToBaseTime(m_oleDateTime_End, &endTime);
    GridData griddata;
    DisplayMessage(L" GetFamilyData Begin!");
    bool result = familyDataController_->GetSingerFamilyData(beginTime, endTime, &griddata);
    if (!result)
    {
        DisplayMessage(L" GetFamilyData failed!");
        return;
    }

    DisplayMessage(L" GetFamilyData success!");

    DisplayDataToGrid(family_columnlist, griddata);
}

void CFamilyDataCenterUIDlg::DisplayDataToGrid(
    const std::vector<std::wstring> columnlist, 
    const GridData& griddata)
{
    if (griddata.empty())
        return;
    m_griddata = griddata;

    //删除之前的数据
    m_ListCtrl_SummaryData.SetItemCountEx(0);
    m_ListCtrl_SummaryData.Invalidate();
    m_ListCtrl_SummaryData.UpdateWindow();

    int nColumnCount = m_ListCtrl_SummaryData.GetHeaderCtrl()->GetItemCount();
    for (int i = nColumnCount - 1; i >= 0; i--)
        m_ListCtrl_SummaryData.DeleteColumn(i);

    uint32 i = 0;
    for (const auto& it : columnlist)
    {
        m_ListCtrl_SummaryData.InsertColumn(i++, it.c_str(), LVCFMT_LEFT, 100);//插入列
    }

    //生成新的数据缓冲区
    int nItemCount = griddata.size();
    m_ListCtrl_SummaryData.SetItemCountEx(nItemCount);
    m_ListCtrl_SummaryData.Invalidate();
}

void CFamilyDataCenterUIDlg::DisplayMessage(const std::wstring& message)
{
    m_list_message.InsertString(index_++, message.c_str());
    UpdateData(FALSE);
}

void CFamilyDataCenterUIDlg::OnBnClickedBtnExportToExcel()
{
    familyDataController_->ExportToTxt();

    DisplayMessage(L" ExportToExcel Begin!");
    bool result = familyDataController_->ExportToExcel();
    
    if (result)
    {
        DisplayMessage(L" Export success!");
    }
    else
    {
        DisplayMessage(L" Export failed!");
    }
}


void CFamilyDataCenterUIDlg::OnBnClickedBtnLogin()
{
    UpdateData(TRUE);
    DisplayMessage(std::wstring(m_username.GetString()) + L" Login Begin!");
    bool result = familyDataController_->Login(m_username.GetString(), m_password.GetString());
    if (!result)
    {
        DisplayMessage(std::wstring(m_username.GetString()) + L" Login failed!");
        return;
    }

    DisplayMessage(std::wstring(m_username.GetString()) + L" Login success!");
    if (m_remember)
    {
        SaveUserInfo(m_username.GetString(), m_password.GetString(), !!m_remember);
    }
    else
    {
        SaveUserInfo(L"", L"", !!m_remember);
    }
}

void CFamilyDataCenterUIDlg::OnLvnGetdispinfoListSummaryData(NMHDR *pNMHDR, LRESULT *pResult)
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    LV_ITEM *pItem = &(pDispInfo)->item;
    if (pItem->mask & LVIF_TEXT)
    {
        //使缓冲区数据与表格子项对应
        pItem->pszText = (LPWSTR)m_griddata[pItem->iItem][pItem->iSubItem].c_str();
    }

    *pResult = 0;
}

//struct SingerDailyData
//{
//    uint32 singerid;
//    std::string date;
//    uint32 onlinecount;         //开播次数
//    uint32 onlineminute;        //累计直播时长（分钟）
//    uint32 effectivecount;      //有效直播次数（大于1个小时）
//    uint32 maxusers;            //直播间最高人气
//    double revenue;             // 星豆收入
//    uint32 blame;               // 周期累计扣分
//};
void CFamilyDataCenterUIDlg::OnBnClickedBtnGetSingerData()
{
    // TODO:  在此添加控件通知处理程序代码
    UpdateData(TRUE);
    if (!m_singer_id)
    {
        AfxMessageBox(L"请输入正确的主播繁星号");
        return;
    }
    base::Time beginTime;
    base::Time endTime;
    OleDateTimeToBaseTime(m_oleDateTime_Begin, &beginTime);
    OleDateTimeToBaseTime(m_oleDateTime_End, &endTime);
    GridData griddata;
    DisplayMessage(L" GetDailyDataBySingerId Begin!");

    bool result = familyDataController_->GetDailyDataBySingerId(m_singer_id, beginTime, 
        endTime, &griddata);

    if (!result)
    {
        DisplayMessage(L" GetDailyDataBySingerId failed!");
        return;
    }
    DisplayMessage(L" GetDailyDataBySingerId success!");

    DisplayDataToGrid(singer_columnlist, griddata);
}


void CFamilyDataCenterUIDlg::OnBnClickedBtnGetNewSinger()
{
    UpdateData(TRUE);
    base::Time beginTime;
    base::Time endTime;
    OleDateTimeToBaseTime(m_oleDateTime_Begin, &beginTime);
    OleDateTimeToBaseTime(m_oleDateTime_End, &endTime);
    GridData griddata;

    familyDataController_->GetFamilyOpenDayCountSummary(beginTime, endTime, &griddata);

    DisplayDataToGrid(family_columnlist, griddata);
}
