#include "main.h"
#include <string>
#include <commctrl.h>

#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Dwmapi.lib")

#define DEMO_WINDOW_CLASS_NAME L"BrowserWindowStyles"
#define DEMO_WINDOW_CLASS_NAME2 L"BrowserWindowStyles2"
#define DEMO_WINDOW_TITLE L"Brave"

// prototypes
WORD initializeWindowClass(WNDPROC eventHandler, HINSTANCE hInstance, std::wstring className, HBRUSH brush);

// globals
// > menu
extern RECT menuRect;
extern void CreateMenus(HMENU parentMenu);
extern LRESULT OnMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
// > non-client area (mouse)
extern LRESULT OnLButtonDownNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
extern LRESULT OnLButtonUpNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
extern LRESULT OnMouseMoveNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
// > non-client area (paint)
extern void RedrawNC(HWND hwnd);
extern LRESULT OnPaintNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
// > non-client area (hit test)
extern LRESULT OnHitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
// > caption button
extern BOOL CalculateMinMaxCloseRect(HWND hWnd);
// > this file
titlebarButton buttons[3];
UINT buttonCommandIds[3] = { 999990, 999991, 999992 };
bool maximized;
bool active;
HMODULE themeFile;
HWND hackWindow;

bool IsWindows10() {
  OSVERSIONINFO osvi;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);

  return osvi.dwMajorVersion == 10;
}

LRESULT OnPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);

	RECT clientRect;
  GetClientRect(hWnd, &clientRect);
	FillRect(hDC, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	EndPaint(hWnd, &ps);

	return NULL;
}

//https://msdn.microsoft.com/en-us/library/windows/desktop/ms632606(v=vs.85).aspx
LRESULT OnCalcSizeNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  // NOTE: this method only overridden currently to calculate the min/max/close RECT sizes
  CalculateMinMaxCloseRect(hWnd);
  // default behavior
  return DefWindowProc(hWnd, WM_NCCALCSIZE, wParam, lParam);
}

LRESULT CALLBACK WndProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    // non-client area
    case WM_NCCALCSIZE:
      return OnCalcSizeNCA(hWnd, wParam, lParam);
    case WM_NCHITTEST:
      return OnHitTestNCA(hWnd, wParam, lParam);
    case WM_NCLBUTTONDOWN:
      return OnLButtonDownNCA(hWnd, wParam, lParam);
    case WM_NCLBUTTONUP:
      return OnLButtonUpNCA(hWnd, wParam, lParam);
    case WM_NCMOUSEMOVE:
      return OnMouseMoveNCA(hWnd, wParam, lParam);

    // These three messages all cause the caption to
    // be repainted, so we have to handle all three to properly
    // support inserted buttons
    case WM_NCACTIVATE:
      active = (wParam != WA_INACTIVE);
      return OnPaintNCA(hWnd, wParam, lParam);
    case WM_SETTEXT:
      return OnPaintNCA(hWnd, wParam, lParam);
    case WM_NCPAINT:
      return OnPaintNCA(hWnd, wParam, lParam);

    // other
    case WM_SIZE:
      CalculateMinMaxCloseRect(hWnd);
      RedrawNC(hWnd);
      break;
    case WM_PAINT:
      return OnPaint(hWnd);
    case WM_COMMAND:
      //caption buttons
      if (wParam == buttonCommandIds[TBB_MINIMIZE]) {
        ShowWindow(hWnd, SW_MINIMIZE);
        return NULL;
      } else if (wParam == buttonCommandIds[TBB_MAXIMIZE]) {
        maximized = !maximized;
        ShowWindow(hWnd, maximized ? SW_MAXIMIZE : SW_SHOWNORMAL);
        return NULL;
      } else if (wParam == buttonCommandIds[TBB_CLOSE]) {
        PostQuitMessage(0);
        return NULL;
      }

      return OnMenu(hWnd, wParam, lParam);

    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
      PostQuitMessage(0);
      return NULL;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WORD result = initializeWindowClass(WndProc, hInstance, DEMO_WINDOW_CLASS_NAME, NULL);
  result = initializeWindowClass(WndProc2, hInstance, DEMO_WINDOW_CLASS_NAME2, (HBRUSH)GetStockObject(GRAY_BRUSH));

  // Findings regarding manifest and enabling of Visual Styles
  // https://msdn.microsoft.com/en-us/library/windows/desktop/dn481241(v=vs.85).aspx
  // https://msdn.microsoft.com/en-us/library/windows/desktop/bb773175(v=vs.85).aspx
  // 
  // The application manifest is required in order for GetVersionEx to properly return info about Windows 10.
  // If absent, the app will always detect major=6, minor=2, build=9200 on Windows 10.
  //
  // Common Controls 6 is enabled by
  // - linking to Comctl32.lib (see project properties > Linker > Input)
  // - calling InitCommonControls()
  // - Adding a dependency entry into the application manifest, specifying version 6
  InitCommonControls();
  bool isWindows10 = IsWindows10();

  // Since the above does not seem to work, my next approach will be to try to get
  // the bitmaps from the msstyles file on disk.
  //themeFile = LoadLibraryEx(TEXT("C:\\Windows\\Resources\\Themes\\aero\\aero.msstyles"), NULL, LOAD_LIBRARY_AS_DATAFILE);

  HMENU menu = CreateMenu();
  CreateMenus(menu);

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


  hackWindow = CreateWindowEx(
    WS_EX_LEFT,
    DEMO_WINDOW_CLASS_NAME2,
    DEMO_WINDOW_TITLE,
    WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
    0,
    0,
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

  //FreeLibrary(themeFile);

  return (int)msg.wParam;
}

WORD initializeWindowClass(WNDPROC eventHandler, HINSTANCE hInstance, std::wstring className, HBRUSH brush) {
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
  windowClass.hbrBackground = brush ? brush : (HBRUSH)GetStockObject(BLACK_BRUSH);
  windowClass.lpszClassName = className.c_str();
  windowClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

  return RegisterClassEx(&windowClass);
}
