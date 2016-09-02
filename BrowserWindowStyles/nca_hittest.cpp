#include "main.h"


extern titlebarButton buttons[3];
extern RECT menuRect;

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms645618(v=vs.85).aspx
LRESULT OnHitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);

  POINT point = { GET_X_LPARAM(lParam) - windowRect.left, GET_Y_LPARAM(lParam) - windowRect.top };

  if (PtInRect(&menuRect, point)) { return HTMENU; }
  if (PtInRect(&buttons[TBB_CLOSE].clientRect, point)) { return HTBORDER; }
  if (PtInRect(&buttons[TBB_MAXIMIZE].clientRect, point)) { return HTBORDER; }
  if (PtInRect(&buttons[TBB_MINIMIZE].clientRect, point)) { return HTBORDER; }

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
