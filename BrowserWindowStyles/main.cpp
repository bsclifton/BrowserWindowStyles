#include <windows.h>
#include <string>
#include <Dwmapi.h>
#include <Uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

// Demo to test win32 styles in preperation for updating Brave look/feel on Windows
// Helpful links:
// https://msdn.microsoft.com/en-us/library/bb384843.aspx
// https://msdn.microsoft.com/en-us/library/bb688195(VS.85).aspx
// http://www.winprog.org/tutorial/menus.html
#define DEMO_WINDOW_CLASS_NAME L"BrowserWindowStyles"
#define DEMO_CHILD_WINDOW_CLASS_NAME  L"BrowserWindowStyles-CHILD"
#define DEMO_WINDOW_TITLE L"Window Title Goes Here"
#define ID_FILE_QUIT 11
#define ID_EDIT_TOGGLE 100

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

//DWMNCRENDERINGPOLICY nonClientRenderingPolicy;
CLIENTCREATESTRUCT MDIClientCreateStruct;
#define IDM_FIRSTCHILD 1
HWND ghMDIClientArea;
RECT parentBounds;
HMENU menu;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CREATE: {
      GetClientRect(hWnd, &parentBounds);
      WINDOWINFO info;
      GetWindowInfo(hWnd, &info);
      
      // attempt to create MDI frame with a menu
      // http://www.codeproject.com/Articles/35634/A-pure-Win-based-MDI-application
      MDIClientCreateStruct.hWindowMenu = NULL;
      MDIClientCreateStruct.idFirstChild = IDM_FIRSTCHILD;
      ghMDIClientArea = CreateWindow(TEXT("MDICLIENT"),
        NULL,
        WS_CHILD | WS_CLIPCHILDREN | WS_SYSMENU | WS_VISIBLE,
        info.rcWindow.left + parentBounds.left,
        info.rcWindow.top + parentBounds.top + 45,
        parentBounds.right - parentBounds.left,
        ((parentBounds.bottom - parentBounds.top) - 45),
        hWnd,
        menu,
        GetModuleHandle(NULL),
        (void*)&MDIClientCreateStruct);

      return NULL;
    }

    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hDC = BeginPaint(hWnd, &ps);

      RECT rectClient;
      GetClientRect(hWnd, &rectClient);
      FillRect(hDC, &rectClient, (HBRUSH)GetStockObject(BLACK_BRUSH));

      //https://msdn.microsoft.com/en-us/library/windows/desktop/bb773210(v=vs.85).aspx

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

      // https://msdn.microsoft.com/en-us/library/aa969233.aspx
      TITLEBARINFOEX info = { sizeof(info) };

      #define BSC_TITLEBAR 0
      #define BSC_RESERVED 1
      #define BSC_MINIMIZE 2
      #define BSC_MAXMIZE 3
      #define BSC_HELP 4
      #define BSC_CLOSE 5
      //https://blogs.msdn.microsoft.com/oldnewthing/20140505-00/?p=1083
      if (!SendMessage(hWnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&info)) {
        return FALSE;
      }
      //could alternatively get size via
      // http://stackoverflow.com/questions/479332/how-to-get-size-and-position-of-window-caption-buttons-minimise-restore-close
      int closeWidth = info.rgrect[BSC_CLOSE].right - info.rgrect[BSC_CLOSE].left;
      int closeHeight = info.rgrect[BSC_CLOSE].bottom - info.rgrect[BSC_CLOSE].top;
      int maximizeWidth = info.rgrect[BSC_MAXMIZE].right - info.rgrect[BSC_MAXMIZE].left;
      int maximizeHeight = info.rgrect[BSC_MAXMIZE].bottom - info.rgrect[BSC_MAXMIZE].top;
      int minimizeWidth = info.rgrect[BSC_MINIMIZE].right - info.rgrect[BSC_MINIMIZE].left;
      int minimizeHeight = info.rgrect[BSC_MINIMIZE].bottom - info.rgrect[BSC_MINIMIZE].top;

      RECT rectCloseButton2;
      rectCloseButton2.left = rectClient.right - closeWidth;
      rectCloseButton2.right = rectClient.right;
      rectCloseButton2.top = 20;
      rectCloseButton2.bottom = rectCloseButton2.top + closeHeight;

      RECT rectMaximizeButton2;
      rectMaximizeButton2.right = rectCloseButton2.left;
      rectMaximizeButton2.left = rectMaximizeButton2.right - maximizeWidth;
      rectMaximizeButton2.top = 20;
      rectMaximizeButton2.bottom = rectMaximizeButton2.top + maximizeHeight;

      RECT rectMinimizeButton2;
      rectMinimizeButton2.right = rectMaximizeButton2.left;
      rectMinimizeButton2.left = rectMinimizeButton2.right - minimizeWidth;
      rectMinimizeButton2.top = 20;
      rectMinimizeButton2.bottom = rectMinimizeButton2.top + minimizeHeight;

      /*
      //another way to get size/bounds for the min/max/close buttons
      if (false) {
        //https://msdn.microsoft.com/en-us/library/windows/desktop/aa969515(v=vs.85).aspx
        RECT bounds;
        DwmGetWindowAttribute(hWnd, DWMWA_CAPTION_BUTTON_BOUNDS, &bounds, sizeof(RECT));
        closeWidth = bounds.right - bounds.left;
        closeHeight = bounds.bottom - bounds.top;
      }*/

      
      //didn't get a chance to look at this all the way

      HTHEME hTheme;
      //hTheme = OpenThemeData(hWnd, L"DWMWINDOW");
      /*RECT rectMin;
      DWORD BufSize;
      BYTE PBuf[1024 * 1024];
      GetThemeRect(hTheme, WP_MINCAPTION, MNCS_ACTIVE, TMT_ATLASRECT, &rectMin);
      HINSTANCE hInstance = GetModuleHandle(NULL);
      GetThemeStream(hTheme, WP_MINCAPTION, MNCS_ACTIVE, TMT_ATLASRECT, (VOID**)&PBuf, &BufSize, hInstance);*/
      //CloseThemeData(hTheme);
     


      //http://stackoverflow.com/questions/34004819/windows-10-close-minimize-and-maximize-buttons
      SetWindowTheme(hWnd, L"EXPLORER", NULL);
      hTheme = OpenThemeData(hWnd, L"WINDOW");
      DrawThemeBackground(hTheme, hDC, WP_CLOSEBUTTON, CBS_NORMAL, &rectCloseButton2, NULL);
      DrawThemeBackground(hTheme, hDC, WP_MAXBUTTON, MAXBS_NORMAL, &rectMaximizeButton2, NULL);
      DrawThemeBackground(hTheme, hDC, WP_MINBUTTON, MINBS_NORMAL, &rectMinimizeButton2, NULL);
      CloseThemeData(hTheme);


      EndPaint(hWnd, &ps);
      return NULL;
    }

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case ID_FILE_QUIT:
          PostQuitMessage(0);
          return NULL;

        //case ID_EDIT_TOGGLE:
          //nonClientRenderingPolicy = nonClientRenderingPolicy == DWMNCRP_DISABLED ? DWMNCRP_ENABLED : DWMNCRP_DISABLED;
          //DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &nonClientRenderingPolicy, sizeof(nonClientRenderingPolicy));
          //return NULL;
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

LRESULT CALLBACK WndProcChild(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_SIZE:
      //resize width/height
      break;
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

  AppendMenu(editMenu, MF_STRING, ID_EDIT_TOGGLE, L"BSC-toggle");

  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)editMenu, L"Edit");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WORD result = initializeWindowClass(WndProc, hInstance, DEMO_WINDOW_CLASS_NAME);
  result = initializeWindowClass(WndProcChild, hInstance, DEMO_CHILD_WINDOW_CLASS_NAME);

  menu = CreateMenu();
  
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
    WS_POPUPWINDOW | WS_CAPTION | WS_CLIPCHILDREN | WS_THICKFRAME,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    640,
    480,
    HWND_DESKTOP,
    NULL,
    hInstance, 0);

  HWND hChild1 = CreateWindowEx(
    WS_EX_LEFT,
    DEMO_CHILD_WINDOW_CLASS_NAME,
    DEMO_WINDOW_TITLE,
    WS_CHILD,
    parentBounds.left,
    parentBounds.top,
    parentBounds.right - parentBounds.left,
    45,
    hWnd,
    NULL,
    hInstance, 0);
  /*
  HWND hChild2 = CreateWindowEx(
    WS_EX_LEFT,
    DEMO_WINDOW_CLASS_NAME,
    DEMO_WINDOW_TITLE,
    WS_CHILD | WS_SYSMENU,
    inside.left,
    inside.top + 45,
    inside.right - inside.left,
    ((inside.bottom - inside.top) - 45),
    hWnd,
    menu,
    hInstance, 0);*/

  MDICREATESTRUCT MDIChildCreateStruct;
  MDIChildCreateStruct.szClass = DEMO_CHILD_WINDOW_CLASS_NAME;
  MDIChildCreateStruct.szTitle = TEXT("New Tab");
  MDIChildCreateStruct.hOwner = hInstance;
  MDIChildCreateStruct.x = CW_USEDEFAULT;
  MDIChildCreateStruct.y = CW_USEDEFAULT;
  MDIChildCreateStruct.cx = CW_USEDEFAULT;
  MDIChildCreateStruct.cy = CW_USEDEFAULT;
  MDIChildCreateStruct.style = WS_MAXIMIZE;
  MDIChildCreateStruct.lParam = 0;
  //create the first MDI child
  HWND m_hwndSystemInformation = (HWND)SendMessage(ghMDIClientArea,
    WM_MDICREATE,
    0,
    (LPARAM)(LPMDICREATESTRUCT)&MDIChildCreateStruct);

  // return if its not possible to create the child window
  //if (NULL == m_hwndSystemInformation) {
  //  return 0;
  //}


  ShowWindow(hWnd, nCmdShow);
  ShowWindow(hChild1, nCmdShow);
  //ShowWindow(hChild2, nCmdShow);
  UpdateWindow(hWnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DestroyWindow(hWnd);
  UnregisterClass(DEMO_WINDOW_CLASS_NAME, hInstance);
  UnregisterClass(DEMO_CHILD_WINDOW_CLASS_NAME, hInstance);

  return (int)msg.wParam;
}
