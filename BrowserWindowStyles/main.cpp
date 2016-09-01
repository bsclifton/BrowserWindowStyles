#include <windows.h>
#include <string>
#include <Dwmapi.h>
#include <Uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <windowsx.h>

#pragma comment(lib, "UxTheme.lib")

// Demo to test win32 styles in preperation for updating Brave look/feel on Windows
// Helpful links:
// https://msdn.microsoft.com/en-us/library/bb384843.aspx
// https://msdn.microsoft.com/en-us/library/bb688195(VS.85).aspx
// http://www.winprog.org/tutorial/menus.html
#define DEMO_WINDOW_CLASS_NAME L"BrowserWindowStyles"
#define DEMO_WINDOW_TITLE L"Brave"
#define ID_FILE_QUIT 11
#define ID_FILE_NEW_TAB 1

// Details about the TITLEBARINFOEX structure:
// https://msdn.microsoft.com/en-us/library/aa969233.aspx
// these values are used for the rgrect field.
#define DEMO_TITLE_BAR_TITLEBAR 0
#define DEMO_TITLE_BAR_RESERVED 1
#define DEMO_TITLE_BAR_MINIMIZE 2
#define DEMO_TITLE_BAR_MAXMIZE 3
#define DEMO_TITLE_BAR_HELP 4
#define DEMO_TITLE_BAR_CLOSE 5

RECT minimizeButtonRect;
RECT maximizeButtonRect;
RECT closeButtonRect;
RECT menuRect;
HRGN menuRegion = NULL;

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
  DrawFrameControl(hDC, &minimizeButtonRect, DFC_CAPTION, DFCS_CAPTIONMIN);
  DrawFrameControl(hDC, &maximizeButtonRect, DFC_CAPTION, DFCS_CAPTIONMAX);
  DrawFrameControl(hDC, &closeButtonRect, DFC_CAPTION, DFCS_CAPTIONCLOSE);
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms645618(v=vs.85).aspx
LRESULT OnHitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);

  POINT point = { GET_X_LPARAM(lParam) - windowRect.left, GET_Y_LPARAM(lParam) - windowRect.top };

  if (PtInRect(&menuRect, point)) { return HTMENU; }
  if (PtInRect(&closeButtonRect, point)) { return HTCLOSE; }
  if (PtInRect(&maximizeButtonRect, point)) { return HTMAXBUTTON; }
  if (PtInRect(&minimizeButtonRect, point)) { return HTMINBUTTON; }

  RECT clientRect;
  GetClientRect(hWnd, &clientRect);

  // Determine if the hit test is for resizing. Default middle (1,1).
  USHORT uRow = 1;
  USHORT uCol = 1;
  bool fOnResizeBorder = false;

  // Determine if the point is at the top or bottom of the window.
  if (point.y >= clientRect.top && point.y < clientRect.top + 5) {
    uRow = 0;
  } else if (point.y <= clientRect.bottom && point.y >= clientRect.bottom - 5) {
    uRow = 2;
  }

  // Determine if the point is at the left or right of the window.
  if (point.x >= clientRect.left && point.x < clientRect.left + 5) {
    uCol = 0; // left side
  } else if (point.x < clientRect.right && point.x >= clientRect.right - 5) {
    uCol = 2; // right side
  }

  // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
  LRESULT hitTests[3][3] =
  {
    { HTTOPLEFT,    HTTOP,     HTTOPRIGHT },
    { HTLEFT,       HTCAPTION, HTRIGHT },
    { HTBOTTOMLEFT, HTBOTTOM,  HTBOTTOMRIGHT },
  };

  return hitTests[uRow][uCol];
}

LRESULT OnPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);

	RECT clientRect;
  GetClientRect(hWnd, &clientRect);
  clientRect.top += menuRect.bottom;
	FillRect(hDC, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	EndPaint(hWnd, &ps);

	return NULL;
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd145212(v=vs.85).aspx
LRESULT OnPaintNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  // Force default painting for non-client area
  LRESULT result = DefWindowProc(hWnd, WM_NCPAINT, wParam, lParam);
  
  // Custom non-client area painting goes here
  RECT windowRect;
  HRGN hrgn;
  GetWindowRect(hWnd, &windowRect);
  if (wParam == 1) {
    hrgn = CreateRectRgnIndirect(&windowRect);
  } else {
    hrgn = (HRGN)wParam;
  }

  // https://msdn.microsoft.com/en-us/library/windows/desktop/dd144873(v=vs.85).aspx
  HDC hDC = GetDCEx(hWnd, hrgn, DCX_WINDOW | DCX_INTERSECTRGN);
  windowRect.left = windowRect.right - ((windowRect.right - windowRect.left) - menuRect.right);
  windowRect.bottom = windowRect.top + menuRect.bottom;
  FillRect(hDC, &windowRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
  RenderViaDrawFrameControl(hDC, windowRect);
  ReleaseDC(hWnd, hDC);

  return NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_SIZE: {
      RECT rectClient;
      GetClientRect(hWnd, &rectClient);
      minimizeButtonRect.left = rectClient.right - 60;
      minimizeButtonRect.right = rectClient.right - 40;
      minimizeButtonRect.top = 0;
      minimizeButtonRect.bottom = 20;
      maximizeButtonRect.left = rectClient.right - 40;
      maximizeButtonRect.right = rectClient.right - 20;
      maximizeButtonRect.top = 0;
      maximizeButtonRect.bottom = 20;
      closeButtonRect.left = rectClient.right - 20;
      closeButtonRect.right = rectClient.right;
      closeButtonRect.top = 0;
      closeButtonRect.bottom = 20;
      } break;
    case WM_NCHITTEST:
      return OnHitTestNCA(hWnd, wParam, lParam);
    case WM_NCPAINT:
      return OnPaintNCA(hWnd, wParam, lParam);
    case WM_PAINT:
      return OnPaint(hWnd);

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case ID_FILE_NEW_TAB: {
          HMENU menu = GetMenu(hWnd);
          MENUBARINFO info;
          info.cbSize = sizeof(MENUBARINFO);
          BOOL result = GetMenuBarInfo(hWnd, OBJID_MENU, 0, &info);
          int i = 4;
          int j = i;
          MessageBeep(250);
        } return NULL;
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
  AppendMenu(fileMenu, MF_STRING, ID_FILE_NEW_TAB, L"New Tab");
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
  AppendMenu(editMenu, MF_STRING, NULL, L"Undo");
  AppendMenu(editMenu, MF_STRING, NULL, L"Redo");
  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)editMenu, L"Edit");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WORD result = initializeWindowClass(WndProc, hInstance, DEMO_WINDOW_CLASS_NAME);

  menuRect.left = 0;
  menuRect.bottom = GetSystemMetrics(SM_CYMENU);
  menuRect.top = 0;
  menuRect.right = 380; //TODO: get proper menu size

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
    WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    640,
    480,
    HWND_DESKTOP,
    menu,
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
