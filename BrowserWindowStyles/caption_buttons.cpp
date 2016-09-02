#include "main.h"

// Details about the TITLEBARINFOEX structure:
// https://msdn.microsoft.com/en-us/library/aa969233.aspx
// these values are used for the rgrect field.
#define DEMO_TITLE_BAR_TITLEBAR 0
#define DEMO_TITLE_BAR_RESERVED 1
#define DEMO_TITLE_BAR_MINIMIZE 2
#define DEMO_TITLE_BAR_MAXMIZE 3
#define DEMO_TITLE_BAR_HELP 4
#define DEMO_TITLE_BAR_CLOSE 5

extern titlebarButton buttons[3];

BOOL CalculateMinMaxCloseRect(HWND hWnd) {
  // Get location of minimize/maximize/close buttons
  // https://blogs.msdn.microsoft.com/oldnewthing/20140505-00/?p=1083
  TITLEBARINFOEX info = { sizeof(info) };
  if (!SendMessage(hWnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&info)) {
    return FALSE;
  }
  //RECT buttonsRect;
  //var sizeButtonsRect = Marshal.SizeOf(buttonsRect);
  //DwmGetWindowAttribute(hWnd, DWMWA_CAPTION_BUTTON_BOUNDS, &buttonsRect, sizeof(buttonsRect));

  // Size was obtained w/ SendMessage call w/ message type WM_GETTITLEBARINFOEX
  // Could have alternatively gotton size/bounds via these methods:
  // GetSystemMetrics - http://stackoverflow.com/questions/479332/how-to-get-size-and-position-of-window-caption-buttons-minimise-restore-close
  // DwmGetWindowAttribute - https://msdn.microsoft.com/en-us/library/windows/desktop/aa969515(v=vs.85).aspx
  int closeWidth = info.rgrect[DEMO_TITLE_BAR_CLOSE].right - info.rgrect[DEMO_TITLE_BAR_CLOSE].left;
  int closeHeight = 20; // info.rgrect[DEMO_TITLE_BAR_CLOSE].bottom - info.rgrect[DEMO_TITLE_BAR_CLOSE].top;
  int maximizeWidth = info.rgrect[DEMO_TITLE_BAR_MAXMIZE].right - info.rgrect[DEMO_TITLE_BAR_MAXMIZE].left;
  int maximizeHeight = 20;// info.rgrect[DEMO_TITLE_BAR_MAXMIZE].bottom - info.rgrect[DEMO_TITLE_BAR_MAXMIZE].top;
  int minimizeWidth = info.rgrect[DEMO_TITLE_BAR_MINIMIZE].right - info.rgrect[DEMO_TITLE_BAR_MINIMIZE].left;
  int minimizeHeight = 20;// info.rgrect[DEMO_TITLE_BAR_MINIMIZE].bottom - info.rgrect[DEMO_TITLE_BAR_MINIMIZE].top;

  RECT clientRect;
  GetClientRect(hWnd, &clientRect);

  RECT* p = &buttons[TBB_CLOSE].clientRect;
  p->left = clientRect.right - closeWidth;
  p->right = clientRect.right;
  p->top = 0;
  p->bottom = p->top + closeHeight;

  RECT* p2 = &buttons[TBB_MAXIMIZE].clientRect;
  p2->right = p->left;
  p2->left = p2->right - maximizeWidth;
  p2->top = 0;
  p2->bottom = p2->top + maximizeHeight;

  p = &buttons[TBB_MINIMIZE].clientRect;
  p->right = p2->left;
  p->left = p->right - minimizeWidth;
  p->top = 0;
  p->bottom = p->top + minimizeHeight;

  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);

  CopyRect(&buttons[TBB_MINIMIZE].screenRect, &buttons[TBB_MINIMIZE].clientRect);
  OffsetRect(&buttons[TBB_MINIMIZE].screenRect, windowRect.left, windowRect.top);

  CopyRect(&buttons[TBB_MAXIMIZE].screenRect, &buttons[TBB_MAXIMIZE].clientRect);
  OffsetRect(&buttons[TBB_MAXIMIZE].screenRect, windowRect.left, windowRect.top);

  CopyRect(&buttons[TBB_CLOSE].screenRect, &buttons[TBB_CLOSE].clientRect);
  OffsetRect(&buttons[TBB_CLOSE].screenRect, windowRect.left, windowRect.top);

  return TRUE;
}
