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

void RenderViaDrawThemeBackground(HWND hWnd, HDC hDC) {
  // Calls I discovered via:
  // http://stackoverflow.com/questions/34004819/windows-10-close-minimize-and-maximize-buttons
  // Reference:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/bb773289(v=vs.85).aspx
  SetWindowTheme(hWnd, L"EXPLORER", NULL);
  HTHEME hTheme = OpenThemeData(hWnd, L"WINDOW");
  DrawThemeBackground(hTheme, hDC, WP_CLOSEBUTTON, CBS_NORMAL, &closeButtonRect, NULL);
  DrawThemeBackground(hTheme, hDC, WP_MAXBUTTON, MAXBS_NORMAL, &maximizeButtonRect, NULL);
  DrawThemeBackground(hTheme, hDC, WP_MINBUTTON, MINBS_NORMAL, &minimizeButtonRect, NULL);
  CloseThemeData(hTheme);
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
  windowRect.bottom = windowRect.top + closeButtonRect.bottom;
  FillRect(hDC, &windowRect, (HBRUSH)GetStockObject(GRAY_BRUSH));
  //RenderViaDrawFrameControl(hDC, windowRect);
  RenderViaDrawThemeBackground(hWnd, hDC);
  ReleaseDC(hWnd, hDC);

  return NULL;
}

BOOL CalculateMinMaxCloseRect(HWND hWnd) {
  // Get location of minimize/maximize/close buttons
  // https://blogs.msdn.microsoft.com/oldnewthing/20140505-00/?p=1083
  TITLEBARINFOEX info = { sizeof(info) };
  if (!SendMessage(hWnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&info)) {
    return FALSE;
  }

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

  RECT rectWindow;
  GetWindowRect(hWnd, &rectWindow);

  closeButtonRect.left = rectWindow.right - closeWidth;
  closeButtonRect.right = rectWindow.right;
  closeButtonRect.top = 0;
  closeButtonRect.bottom = closeButtonRect.top + closeHeight;

  maximizeButtonRect.right = closeButtonRect.left;
  maximizeButtonRect.left = maximizeButtonRect.right - maximizeWidth;
  maximizeButtonRect.top = 0;
  maximizeButtonRect.bottom = maximizeButtonRect.top + maximizeHeight;

  minimizeButtonRect.right = maximizeButtonRect.left;
  minimizeButtonRect.left = minimizeButtonRect.right - minimizeWidth;
  minimizeButtonRect.top = 0;
  minimizeButtonRect.bottom = minimizeButtonRect.top + minimizeHeight;
}

//https://msdn.microsoft.com/en-us/library/windows/desktop/ms632606(v=vs.85).aspx
LRESULT OnCalcSizeNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  CalculateMinMaxCloseRect(hWnd);

  if (wParam) {
    //the first rectangle contains the new coordinates of a window that has been moved or resized, that is, it is the proposed new window coordinates
    //The second contains the coordinates of the window before it was moved or resized.
    //The third contains the coordinates of the window's client area before the window was moved or resized.
    //
    //If the window is a child window, the coordinates are relative to the client area of the parent window.
    //If the window is a top-level window, the coordinates are relative to the screen origin.
    //
    //When the window procedure returns, the first rectangle contains the coordinates of the new client rectangle resulting from the move or resize.
    //The second rectangle contains the valid destination rectangle, and the third rectangle contains the valid source rectangle.
    //The last two rectangles are used in conjunction with the return value of the WM_NCCALCSIZE message to determine the area of the window to be preserved.
    //
    NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
    params->rgrc[0].top = closeButtonRect.bottom;
    return WVR_VALIDRECTS;
    //return DefWindowProc(hWnd, WM_NCCALCSIZE, wParam, lParam);

  } else {
    // On entry, the structure contains the proposed window rectangle for the window.
    // On exit, the structure should contain the screen coordinates of the corresponding window client area.
    RECT* rect = (LPRECT)lParam;
    rect->top = closeButtonRect.bottom;
    return WVR_VALIDRECTS;
  }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_SIZE:
      CalculateMinMaxCloseRect(hWnd);
      break;
    case WM_NCCALCSIZE:
      return OnCalcSizeNCA(hWnd, wParam, lParam);
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
