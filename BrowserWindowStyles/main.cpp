#include <windows.h>
#include <string>
#include <Dwmapi.h>
#include <Uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

#pragma comment(lib, "UxTheme.lib")

// Demo to test win32 styles in preperation for updating Brave look/feel on Windows
// Helpful links:
// https://msdn.microsoft.com/en-us/library/bb384843.aspx
// https://msdn.microsoft.com/en-us/library/bb688195(VS.85).aspx
// http://www.winprog.org/tutorial/menus.html
#define DEMO_WINDOW_CLASS_NAME L"BrowserWindowStyles"
#define DEMO_WINDOW_TITLE L"Brave"
#define ID_FILE_QUIT 11

// Details about the TITLEBARINFOEX structure:
// https://msdn.microsoft.com/en-us/library/aa969233.aspx
// these values are used for the rgrect field.
#define DEMO_TITLE_BAR_TITLEBAR 0
#define DEMO_TITLE_BAR_RESERVED 1
#define DEMO_TITLE_BAR_MINIMIZE 2
#define DEMO_TITLE_BAR_MAXMIZE 3
#define DEMO_TITLE_BAR_HELP 4
#define DEMO_TITLE_BAR_CLOSE 5

WORD initializeWindowClass(WNDPROC eventHandler, HINSTANCE hInstance, std::wstring className) {
  WNDCLASSEX windowClass;

  memset(&windowClass, 0, sizeof(WNDCLASSEX));

  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
  windowClass.lpfnWndProc = eventHandler;
  windowClass.cbClsExtra = 0;
  windowClass.cbWndExtra = 0;
  windowClass.hInstance = hInstance;
  windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  windowClass.lpszClassName = className.c_str();
  windowClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  
  return RegisterClassEx(&windowClass);
}

void RenderViaDrawFrameControl(HDC hDC, RECT& rectClient) {
	// DrawFrameControl - parts and states reference
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb773210(v=vs.85).aspx
	RECT rectMinimizeButton;
	rectMinimizeButton.left = rectClient.right - 60;
	rectMinimizeButton.right = rectClient.right - 40;
	rectMinimizeButton.top = 0;
	rectMinimizeButton.bottom = 20;
	DrawFrameControl(hDC, &rectMinimizeButton, DFC_CAPTION, DFCS_CAPTIONMIN);

	RECT rectMaximizeButton;
	rectMaximizeButton.left = rectClient.right - 40;
	rectMaximizeButton.right = rectClient.right - 20;
	rectMaximizeButton.top = 0;
	rectMaximizeButton.bottom = 20;
	DrawFrameControl(hDC, &rectMaximizeButton, DFC_CAPTION, DFCS_CAPTIONMAX);

	RECT rectCloseButton;
	rectCloseButton.left = rectClient.right - 20;
	rectCloseButton.right = rectClient.right;
	rectCloseButton.top = 0;
	rectCloseButton.bottom = 20;
	DrawFrameControl(hDC, &rectCloseButton, DFC_CAPTION, DFCS_CAPTIONCLOSE);
}

void RenderViaDrawThemeBackground(HWND hWnd, HDC hDC, RECT& rectClient, TITLEBARINFOEX& info) {
	// Size was obtained w/ SendMessage call w/ message type WM_GETTITLEBARINFOEX
	// Could have alternatively gotton size/bounds via these methods:
	// GetSystemMetrics - http://stackoverflow.com/questions/479332/how-to-get-size-and-position-of-window-caption-buttons-minimise-restore-close
	// DwmGetWindowAttribute - https://msdn.microsoft.com/en-us/library/windows/desktop/aa969515(v=vs.85).aspx
	int closeWidth = info.rgrect[DEMO_TITLE_BAR_CLOSE].right - info.rgrect[DEMO_TITLE_BAR_CLOSE].left;
	int closeHeight = info.rgrect[DEMO_TITLE_BAR_CLOSE].bottom - info.rgrect[DEMO_TITLE_BAR_CLOSE].top;
	int maximizeWidth = info.rgrect[DEMO_TITLE_BAR_MAXMIZE].right - info.rgrect[DEMO_TITLE_BAR_MAXMIZE].left;
	int maximizeHeight = info.rgrect[DEMO_TITLE_BAR_MAXMIZE].bottom - info.rgrect[DEMO_TITLE_BAR_MAXMIZE].top;
	int minimizeWidth = info.rgrect[DEMO_TITLE_BAR_MINIMIZE].right - info.rgrect[DEMO_TITLE_BAR_MINIMIZE].left;
	int minimizeHeight = info.rgrect[DEMO_TITLE_BAR_MINIMIZE].bottom - info.rgrect[DEMO_TITLE_BAR_MINIMIZE].top;

	RECT rectCloseButton;
	rectCloseButton.left = rectClient.right - closeWidth;
	rectCloseButton.right = rectClient.right;
	rectCloseButton.top = 20;
	rectCloseButton.bottom = rectCloseButton.top + closeHeight;

	RECT rectMaximizeButton;
	rectMaximizeButton.right = rectCloseButton.left;
	rectMaximizeButton.left = rectMaximizeButton.right - maximizeWidth;
	rectMaximizeButton.top = 20;
	rectMaximizeButton.bottom = rectMaximizeButton.top + maximizeHeight;

	RECT rectMinimizeButton;
	rectMinimizeButton.right = rectMaximizeButton.left;
	rectMinimizeButton.left = rectMinimizeButton.right - minimizeWidth;
	rectMinimizeButton.top = 20;
	rectMinimizeButton.bottom = rectMinimizeButton.top + minimizeHeight;

	// Calls I discovered via:
	// http://stackoverflow.com/questions/34004819/windows-10-close-minimize-and-maximize-buttons
	// Reference:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb773289(v=vs.85).aspx
	SetWindowTheme(hWnd, L"EXPLORER", NULL);
	HTHEME hTheme = OpenThemeData(hWnd, L"WINDOW");
	DrawThemeBackground(hTheme, hDC, WP_CLOSEBUTTON, CBS_NORMAL, &rectCloseButton, NULL);
	DrawThemeBackground(hTheme, hDC, WP_MAXBUTTON, MAXBS_NORMAL, &rectMaximizeButton, NULL);
	DrawThemeBackground(hTheme, hDC, WP_MINBUTTON, MINBS_NORMAL, &rectMinimizeButton, NULL);
	CloseThemeData(hTheme);
}

LRESULT OnPaint(HWND hWnd) {
	// Get location of minimize/maximize/close buttons
	// https://blogs.msdn.microsoft.com/oldnewthing/20140505-00/?p=1083
	TITLEBARINFOEX info = { sizeof(info) };
	if (!SendMessage(hWnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&info)) {
		return FALSE;
	}

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);

	RECT rectClient;
	GetClientRect(hWnd, &rectClient);
	FillRect(hDC, &rectClient, (HBRUSH)GetStockObject(BLACK_BRUSH));

	RenderViaDrawFrameControl(hDC, rectClient);
	RenderViaDrawThemeBackground(hWnd, hDC, rectClient, info);

	EndPaint(hWnd, &ps);

	return NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_PAINT: {
		return OnPaint(hWnd);
    }

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case ID_FILE_QUIT:
          PostQuitMessage(0);
          return NULL;
      }
      break;

    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
      PostQuitMessage(0);
      return NULL;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AppendFileMenu(HMENU parentMenu) {
  HMENU fileMenu = CreateMenu();
  AppendMenu(fileMenu, MF_STRING, NULL, L"New Tab");
  AppendMenu(fileMenu, MF_STRING, NULL, L"New Private Tab");
  AppendMenu(fileMenu, MF_STRING, NULL, L"New Session Tab");
  AppendMenu(fileMenu, MF_STRING, NULL, L"New Window");
  AppendMenu(fileMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(fileMenu, MF_STRING, NULL, L"Open File...");
  AppendMenu(fileMenu, MF_STRING, NULL, L"Open Location...");
  AppendMenu(fileMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(fileMenu, MF_STRING, NULL, L"Close");
  AppendMenu(fileMenu, MF_STRING, NULL, L"Close Window");
  AppendMenu(fileMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(fileMenu, MF_STRING, NULL, L"Save Page as...");
  AppendMenu(fileMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(fileMenu, MF_STRING, NULL, L"Print...");
  AppendMenu(fileMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(fileMenu, MF_STRING, ID_FILE_QUIT, L"Quit Brave");
  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)fileMenu, L"File");
}

void AppendEditMenu(HMENU parentMenu) {
  HMENU editMenu = CreateMenu();
  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)editMenu, L"Edit");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WORD result = initializeWindowClass(WndProc, hInstance, DEMO_WINDOW_CLASS_NAME);

  HMENU menu = CreateMenu();
  
  //Menu functions
  //https://msdn.microsoft.com/en-us/library/windows/desktop/ff468865(v=vs.85).aspx
  AppendFileMenu(menu);
  AppendEditMenu(menu);
  AppendMenu(menu, NULL, NULL, L"View");
  AppendMenu(menu, NULL, NULL, L"History");
  AppendMenu(menu, NULL, NULL, L"Bookmarks");
  AppendMenu(menu, NULL, NULL, L"Bravery");
  AppendMenu(menu, NULL, NULL, L"Window");
  AppendMenu(menu, NULL, NULL, L"Help");

  HWND hWnd = CreateWindowEx(
    WS_EX_LEFT,
    DEMO_WINDOW_CLASS_NAME,
    DEMO_WINDOW_TITLE,
    WS_POPUPWINDOW | WS_CAPTION | WS_CLIPCHILDREN | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    640,
    480,
    HWND_DESKTOP,
    NULL,
    hInstance, 0);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DestroyWindow(hWnd);
  UnregisterClass(DEMO_WINDOW_CLASS_NAME, hInstance);

  return (int)msg.wParam;
}
