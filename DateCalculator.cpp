/*
2015-08-14 Release 1.0 Terry
- Apply Win32 api to re-create the Date Calculator from c#.
- Validated to build using VC6 with PSDK 2003.
*/

#include <Windows.h>
#include <tchar.h>
#include <string>

#define _SECOND ((INT64) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

#define IDC_BTNWEEKDAY (WM_USER + 1)
#define IDC_BTNWEEKEND (WM_USER + 2)
#define IDC_BTNGETENDDATE (WM_USER + 3)
#define IDC_BTNGETSTARTDATE (WM_USER + 4)
#define IDC_BTNGETEPISODES (WM_USER + 5)
#define IDC_EDTSTARTMONTH (WM_USER + 6)
#define IDC_EDTSTARTDAY (WM_USER + 7)
#define IDC_EDTSTARTYEAR (WM_USER + 8)
#define IDC_EDTENDMONTH (WM_USER + 9)
#define IDC_EDTENDDAY (WM_USER + 10)
#define IDC_EDTENDYEAR (WM_USER + 11)
#define IDC_EDTEPISODES (WM_USER + 12)
#define IDC_EDTPREEMPTIONS (WM_USER + 13)

typedef std::basic_string<TCHAR> ustring;

enum{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};

LPCTSTR szDay[7] = { TEXT("Sunday"), TEXT("Monday"), TEXT("Tuesday"), TEXT("Wednesday"), TEXT("Thursday"), TEXT("Friday"), TEXT("Saturday") };
LPCTSTR szMonth[12] = { TEXT("Jan"), TEXT("Feb"), TEXT("Mar"), TEXT("Apr"), TEXT("May"), TEXT("Jun"), TEXT("Jul"), TEXT("Aug"), TEXT("Sep"), TEXT("Oct"), TEXT("Nov"), TEXT("Dec") };

class AutoSelectEdit
{
private:
	HWND hWnd;
public:
	HWND Window() const
	{
		return hWnd;
	}
	static LRESULT CALLBACK SelEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN:
			OutputDebugString(TEXT("Mouseclickedup\n"));
			PostMessage(hWnd, EM_SETSEL, 0, -1);
			break;
		case WM_SETFOCUS:
			OutputDebugString(TEXT("WM_SETFOCUS\n"));
			PostMessage(hWnd, EM_SETSEL, 0, -1);
			break;
		default:
			break;
		}
		return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA), hWnd, msg, wParam, lParam);
	}
	ustring GetText() const
	{
		int length = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
		TCHAR* buffer = new TCHAR[length + 1];
		SendMessage(hWnd, WM_GETTEXT, length + 1, (LPARAM)buffer);
		ustring str(buffer);
		delete[] buffer;
		return str;
	}
	int GetTextValue()
	{
		int len = GetWindowTextLength(hWnd);
		TCHAR *buffer = new TCHAR[len + 1];
		GetWindowText(hWnd, buffer, len + 1);
		//len = _tstoi(buffer);
		_stscanf(buffer, TEXT("%d"), &len);
		delete[] buffer;
		return len;
	}
	void SetText(ustring str)
	{
		SetWindowText(hWnd, str.c_str());
	}
	HWND Create(LPCTSTR text, HWND parent, int x, int y, int width, int height)
	{
		hWnd = CreateWindow(TEXT("edit"), text, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER, x, y, width, height, parent, NULL, GetModuleHandle(NULL), NULL);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)(AutoSelectEdit::SelEditProc)));
		return hWnd;
	}
};
class DayEpisodeControl
{
private:
	HWND hWnd;
	HWND hWndEpisodesDay;
	HWND hWndEpisodesB2B;
	AutoSelectEdit ASE;
public:
	static LRESULT CALLBACK DayEpisodeProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		DayEpisodeControl *dec = (DayEpisodeControl*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		switch (msg)
		{
		case WM_NCCREATE:
			OutputDebugString(TEXT("WM_NCCREATE\n"));
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return 1;
		case WM_CREATE:
			OutputDebugString(TEXT("WM_CREATE\n"));
			dec->hWndEpisodesDay = CreateWindow(TEXT("button"), ((LPCREATESTRUCT)lParam)->lpszName, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
			dec->hWndEpisodesB2B = CreateWindow(TEXT("button"), TEXT("Back-to-back"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 140, 0, 110, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
			dec->ASE.Create(TEXT("1"), hWnd, 105, 0, 30, 20);
			EnableWindow(dec->ASE.Window(), FALSE);
			return 0;
		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if ((HWND)lParam == dec->hWndEpisodesB2B)
				{
					if (SendMessage(dec->hWndEpisodesB2B, BM_GETCHECK, 0, 0))
					{
						EnableWindow(dec->ASE.Window(), TRUE);
						SetWindowText(dec->ASE.Window(), TEXT("2"));
						SendMessage(dec->hWndEpisodesDay, BM_SETCHECK, BST_CHECKED, 0);
					}
					else
					{
						EnableWindow(dec->ASE.Window(), FALSE);
						SetWindowText(dec->ASE.Window(), TEXT("1"));
					}
					return 0;
				}
				else if ((HWND)lParam == dec->hWndEpisodesDay)
				{
					if (!SendMessage(dec->hWndEpisodesDay, BM_GETCHECK, 0, 0))
					{
						EnableWindow(dec->ASE.Window(), FALSE);
						SetWindowText(dec->ASE.Window(), TEXT("1"));
						SendMessage(dec->hWndEpisodesB2B, BM_SETCHECK, BST_UNCHECKED, 0);
					}
					return 0;
				}
			default:
				break;
			}
			break;
		default:
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	HWND Window() const
	{
		return hWnd;
	}
	HWND Create(LPCTSTR text, HWND parent, int x, int y)
	{
		WNDCLASS wnd = { 0 };
		wnd.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
		wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wnd.hInstance = GetModuleHandle(NULL);
		wnd.lpfnWndProc = DayEpisodeProc;
		wnd.lpszClassName = TEXT("DAYEPISODECLASS");
		wnd.style = CS_VREDRAW | CS_HREDRAW;

		RegisterClass(&wnd);

		return CreateWindow(TEXT("DAYEPISODECLASS"), text, WS_CHILD | WS_VISIBLE, x, y, 250, 20, parent, NULL, GetModuleHandle(NULL), this);
	}
	void SetCheck(BOOL state)
	{
		if (state)
		{
			SendMessage(hWndEpisodesDay, BM_SETCHECK, BST_CHECKED, NULL);
		}
		else
		{
			SendMessage(hWndEpisodesDay, BM_SETCHECK, BST_UNCHECKED, NULL);
			SendMessage(hWndEpisodesB2B, BM_SETCHECK, BST_UNCHECKED, NULL);
			SetWindowText(ASE.Window(), TEXT("1"));
			EnableWindow(ASE.Window(), FALSE);
		}
	}
	BOOL IsDayChecked()
	{
		return SendMessage(hWndEpisodesDay, BM_GETCHECK, 0, 0);
	}
	BOOL IsBackToBackCheck()
	{
		return SendMessage(hWndEpisodesB2B, BM_GETCHECK, 0, 0);
	}
	int GetEpisodes()
	{
		return ASE.GetTextValue();
	}
};

AutoSelectEdit ASE_STARTDAY;
AutoSelectEdit ASE_STARTMONTH;
AutoSelectEdit ASE_STARTYEAR;

AutoSelectEdit ASE_ENDDAY;
AutoSelectEdit ASE_ENDMONTH;
AutoSelectEdit ASE_ENDYEAR;

AutoSelectEdit ASE_EPISODES;
AutoSelectEdit ASE_PREEMPTIONS;

DayEpisodeControl DEC[7];

HWND static_enddate;
HWND static_startdate;

void SetToday()
{
	TCHAR *buffer = new TCHAR[32];
	SYSTEMTIME today = { 0 };
	GetSystemTime(&today);

	wsprintf(buffer, TEXT("%s, %d %s %d"), ustring(szDay[today.wDayOfWeek], 3).c_str(), today.wDay, ustring(szMonth[today.wMonth - 1]).c_str(), today.wYear);
	ustring todaystr(buffer);
	SetWindowText(static_startdate, todaystr.c_str());
	SetWindowText(static_enddate, todaystr.c_str());

	wsprintf(buffer, TEXT("%d"), today.wDay);
	ustring day(buffer);
	ASE_STARTDAY.SetText(day);
	ASE_ENDDAY.SetText(day);
	ZeroMemory(buffer, sizeof(buffer));
	wsprintf(buffer, TEXT("%d"), today.wMonth);
	ustring month(buffer);
	ASE_STARTMONTH.SetText(month);
	ASE_ENDMONTH.SetText(month);
	ZeroMemory(buffer, sizeof(buffer));
	wsprintf(buffer, TEXT("%d"), today.wYear);
	ustring year(buffer);
	ASE_STARTYEAR.SetText(year);
	ASE_ENDYEAR.SetText(year);
	delete[] buffer;
}
void CreateControls(HWND hWnd)
{
	int j;
	for (int i = 0; i < 7; i++)
	{
		j = (i + 1) % 7;
		DEC[j].Create(szDay[j], hWnd, 0, i * 20);
	}

	CreateWindow(TEXT("static"), TEXT("Start date: "), WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 140, 80, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	ASE_STARTMONTH.Create(TEXT("1"), hWnd,90, 140, 25, 20);
	ASE_STARTDAY.Create(TEXT("1"), hWnd, 125, 140, 25, 20);
	ASE_STARTYEAR.Create(TEXT("2015"), hWnd, 160, 140, 45, 20);
	static_startdate = CreateWindow(TEXT("static"), TEXT("<START DATE>"), WS_CHILD | WS_VISIBLE | SS_CENTER, 215, 140, 135, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);

	CreateWindow(TEXT("static"), TEXT("End date: "), WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 160, 80, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	ASE_ENDMONTH.Create(TEXT("1"), hWnd,90, 160, 25, 20);
	ASE_ENDDAY.Create(TEXT("1"), hWnd, 125, 160, 25, 20);
	ASE_ENDYEAR.Create(TEXT("2015"), hWnd, 160, 160, 45, 20);
	static_enddate = CreateWindow(TEXT("static"), TEXT("Wed, 31 Dec 2014"), WS_CHILD | WS_VISIBLE | SS_CENTER, 215, 160, 135, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);

	CreateWindow(TEXT("static"), TEXT("Episodes: "), WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 180, 90, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	ASE_EPISODES.Create(TEXT("1"), hWnd, 100, 180, 40, 20);

	CreateWindow(TEXT("static"), TEXT("Pre-emptions: "), WS_CHILD | WS_VISIBLE | SS_RIGHT, 150, 180, 90, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	ASE_PREEMPTIONS.Create(TEXT("0"), hWnd, 250, 180, 40, 20);

	CreateWindow(TEXT("button"), TEXT("Weekday"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 250, 0, 100, 100, hWnd, (HMENU)IDC_BTNWEEKDAY, GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("Weekend"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 250, 100, 100, 40, hWnd, (HMENU)IDC_BTNWEEKEND, GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("Get End Date"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 200, 110, 40, hWnd, (HMENU)IDC_BTNGETENDDATE, GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("Get Start Date"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 120, 200, 110, 40, hWnd, (HMENU)IDC_BTNGETSTARTDATE, GetModuleHandle(NULL), NULL);
	CreateWindow(TEXT("button"), TEXT("Get Episodes"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 200, 110, 40, hWnd, (HMENU)IDC_BTNGETEPISODES, GetModuleHandle(NULL), NULL);

	SetToday();
}
void PerformWeekDaysSelection()
{
	if (DEC[Monday].IsDayChecked() || DEC[Tuesday].IsDayChecked() || DEC[Wednesday].IsDayChecked() || DEC[Thursday].IsDayChecked() || DEC[Friday].IsDayChecked())
	{
		DEC[Monday].SetCheck(FALSE);
		DEC[Tuesday].SetCheck(FALSE);
		DEC[Wednesday].SetCheck(FALSE);
		DEC[Thursday].SetCheck(FALSE);
		DEC[Friday].SetCheck(FALSE);
	}
	else
	{
		DEC[Monday].SetCheck(TRUE);
		DEC[Tuesday].SetCheck(TRUE);
		DEC[Wednesday].SetCheck(TRUE);
		DEC[Thursday].SetCheck(TRUE);
		DEC[Friday].SetCheck(TRUE);
	}
}
void PerformWeekEndsSelection()
{
	if (DEC[Sunday].IsDayChecked() || DEC[Saturday].IsDayChecked())
	{
		DEC[Sunday].SetCheck(FALSE);
		DEC[Saturday].SetCheck(FALSE);
	}
	else
	{
		DEC[Sunday].SetCheck(TRUE);
		DEC[Saturday].SetCheck(TRUE);
	}
}
void GetNumberOfEpisodes(int startDay, int startMonth, int startYear, int endDay, int endMonth, int endYear, int pre_empt)
{
	int numEpisodes = 0;
	SYSTEMTIME start = { startYear, startMonth, 0, startDay, 0, 0, 0, 0 };
	SYSTEMTIME end = { endYear, endMonth, 0, endDay, 0, 0, 0, 0 };

	FILETIME ft_start;
	FILETIME ft_end;
	SystemTimeToFileTime(&start, &ft_start);
	SystemTimeToFileTime(&end, &ft_end);
	ULONGLONG qwStart, qwEnd;

	// Copy the time into a quadword.
	qwStart = (((ULONGLONG)ft_start.dwHighDateTime) << 32) + ft_start.dwLowDateTime;
	qwEnd = (((ULONGLONG)ft_end.dwHighDateTime) << 32) + ft_end.dwLowDateTime;

	while (qwStart <= qwEnd)
	{

		ft_start.dwLowDateTime = (DWORD)(qwStart & 0xFFFFFFFF);
		ft_start.dwHighDateTime = (DWORD)(qwStart >> 32);
		FileTimeToSystemTime(&ft_start, &start);
		if (DEC[start.wDayOfWeek].IsDayChecked())
		{
			numEpisodes = numEpisodes + DEC[start.wDayOfWeek].GetEpisodes();
		}
		qwStart += 1 * _DAY;
	}
	TCHAR *buffer = new TCHAR[16];
	wsprintf(buffer, TEXT("%d"), numEpisodes - pre_empt);
	ustring str(buffer);
	delete[] buffer;
	ASE_EPISODES.SetText(str);
}
void GetEndDate(int startDay, int startMonth, int startYear, int episodes, int pre_empt)
{
	TCHAR *buffer = new TCHAR[32];
	SYSTEMTIME start = { startYear, startMonth, 0, startDay, 0, 0, 0, 0 };
	FILETIME ft_start;
	SystemTimeToFileTime(&start, &ft_start);
	FileTimeToSystemTime(&ft_start, &start);
	ULONGLONG qwStart;
	
	wsprintf(buffer, TEXT("%s, %d %s %d"), ustring(szDay[start.wDayOfWeek], 3).c_str(), start.wDay, ustring(szMonth[start.wMonth - 1]).c_str(), start.wYear);
	ustring startdate(buffer);
	SetWindowText(static_startdate, startdate.c_str());

	qwStart = (((ULONGLONG)ft_start.dwHighDateTime) << 32) + ft_start.dwLowDateTime;
	if (DEC[Sunday].IsDayChecked() || DEC[Monday].IsDayChecked() || DEC[Tuesday].IsDayChecked() || DEC[Wednesday].IsDayChecked() || DEC[Thursday].IsDayChecked() || DEC[Friday].IsDayChecked() || DEC[Saturday].IsDayChecked())
	{
		episodes = episodes + pre_empt;
		while (episodes > 0)
		{
			ft_start.dwLowDateTime = (DWORD)(qwStart & 0xFFFFFFFF);
			ft_start.dwHighDateTime = (DWORD)(qwStart >> 32);
			FileTimeToSystemTime(&ft_start, &start);
			if (DEC[start.wDayOfWeek].IsDayChecked())
			{
				episodes = episodes - DEC[start.wDayOfWeek].GetEpisodes();
			}
			qwStart += 1 * _DAY;
		}
		
		wsprintf(buffer, TEXT("%s, %d %s %d"), ustring(szDay[start.wDayOfWeek], 3).c_str(), start.wDay, ustring(szMonth[start.wMonth - 1]).c_str(), start.wYear);
		ustring enddate(buffer);
		SetWindowText(static_enddate, enddate.c_str());

		wsprintf(buffer, TEXT("%d"), start.wDay);
		ustring day(buffer);
		ASE_ENDDAY.SetText(day);
		ZeroMemory(buffer, sizeof(buffer));
		wsprintf(buffer, TEXT("%d"), start.wMonth);
		ustring month(buffer);
		ASE_ENDMONTH.SetText(month);
		ZeroMemory(buffer, sizeof(buffer));
		wsprintf(buffer, TEXT("%d"), start.wYear);
		ustring year(buffer);
		ASE_ENDYEAR.SetText(year);
		
	}
	else
	{
		MessageBox(NULL, TEXT("Please select at least one day."), TEXT("Error"), MB_OK);
	}
	delete[] buffer;
}
void GetStartDate(int endDay, int endMonth, int endYear, int episodes, int pre_empt)
{
	TCHAR *buffer = new TCHAR[32];
	SYSTEMTIME end = { endYear, endMonth, 0, endDay, 0, 0, 0, 0 };
	FILETIME ft_end;
	SystemTimeToFileTime(&end, &ft_end);
	FileTimeToSystemTime(&ft_end, &end);
	ULONGLONG qwStart;
	
	wsprintf(buffer, TEXT("%s, %d %s %d"), ustring(szDay[end.wDayOfWeek], 3).c_str(), end.wDay, ustring(szMonth[end.wMonth - 1]).c_str(), end.wYear);
	ustring enddate(buffer);
	SetWindowText(static_enddate, enddate.c_str());

	qwStart = (((ULONGLONG)ft_end.dwHighDateTime) << 32) + ft_end.dwLowDateTime;
	if (DEC[Sunday].IsDayChecked() || DEC[Monday].IsDayChecked() || DEC[Tuesday].IsDayChecked() || DEC[Wednesday].IsDayChecked() || DEC[Thursday].IsDayChecked() || DEC[Friday].IsDayChecked() || DEC[Saturday].IsDayChecked())
	{
		episodes = episodes + pre_empt;
		while (episodes > 0)
		{
			ft_end.dwLowDateTime = (DWORD)(qwStart & 0xFFFFFFFF);
			ft_end.dwHighDateTime = (DWORD)(qwStart >> 32);
			FileTimeToSystemTime(&ft_end, &end);
			if (DEC[end.wDayOfWeek].IsDayChecked())
			{
				episodes = episodes - DEC[end.wDayOfWeek].GetEpisodes();
			}
			qwStart -= 1 * _DAY;
		}
		
		wsprintf(buffer, TEXT("%s, %d %s %d"), ustring(szDay[end.wDayOfWeek], 3).c_str(), end.wDay, ustring(szMonth[end.wMonth - 1]).c_str(), end.wYear);
		ustring startdate(buffer);
		SetWindowText(static_startdate, startdate.c_str());

		wsprintf(buffer, TEXT("%d"), end.wDay);
		ustring day(buffer);
		ASE_STARTDAY.SetText(day);
		ZeroMemory(buffer, sizeof(buffer));
		wsprintf(buffer, TEXT("%d"), end.wMonth);
		ustring month(buffer);
		ASE_STARTMONTH.SetText(month);
		ZeroMemory(buffer, sizeof(buffer));
		wsprintf(buffer, TEXT("%d"), end.wYear);
		ustring year(buffer);
		ASE_STARTYEAR.SetText(year);
		
	}
	else
	{
		MessageBox(NULL, TEXT("Please select at least one day."), TEXT("Error"), MB_OK);
	}
	delete[] buffer;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		CreateControls(hWnd);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BTNWEEKDAY:
			PerformWeekDaysSelection();
			return 0;
		case IDC_BTNWEEKEND:
			PerformWeekEndsSelection();
			return 0;
		case IDC_BTNGETEPISODES:
			GetNumberOfEpisodes(ASE_STARTDAY.GetTextValue(), ASE_STARTMONTH.GetTextValue(), ASE_STARTYEAR.GetTextValue(), ASE_ENDDAY.GetTextValue(), ASE_ENDMONTH.GetTextValue(), ASE_ENDYEAR.GetTextValue(), ASE_PREEMPTIONS.GetTextValue());
			return 0;
		case IDC_BTNGETENDDATE:
			GetEndDate(ASE_STARTDAY.GetTextValue(), ASE_STARTMONTH.GetTextValue(), ASE_STARTYEAR.GetTextValue(), ASE_EPISODES.GetTextValue(), ASE_PREEMPTIONS.GetTextValue());
			return 0;
		case IDC_BTNGETSTARTDATE:
			GetStartDate(ASE_ENDDAY.GetTextValue(), ASE_ENDMONTH.GetTextValue(), ASE_ENDYEAR.GetTextValue(), ASE_EPISODES.GetTextValue(), ASE_PREEMPTIONS.GetTextValue());
			return 0;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdStr, int iCmdLine)
{
	WNDCLASS wnd = { 0 };
	wnd.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = TEXT("CLASSNAME");
	wnd.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wnd);

	RECT clientrect = { 0, 0, 350, 240 };
	AdjustWindowRect(&clientrect, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, FALSE);

	HWND hWndMain = CreateWindow(TEXT("CLASSNAME"), TEXT("Date Calculator"), WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top, NULL, NULL, hInstance, NULL);

	ShowWindow(hWndMain, iCmdLine);
	UpdateWindow(hWndMain);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}