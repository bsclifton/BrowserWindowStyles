#include "main.h"
#include <Dwmapi.h>
#include <vssym32.h>
#include <vsstyle.h>

extern titlebarButton buttons[3];
extern bool maximized;
extern bool active;
extern RECT menuRect;
extern HWND hackWindow;

void RedrawNC(HWND hwnd) {
  SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME);
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd145212(v=vs.85).aspx
LRESULT OnPaintNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  RECT windowRect;
  HRGN hRgn = NULL;
  GetWindowRect(hWnd, &windowRect);
  if (wParam == 1) {
    hRgn = CreateRectRgnIndirect(&windowRect);
  } else {
    hRgn = (HRGN)wParam;
  }

  if (hRgn) {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    RECT captionAreaRect;
    DwmGetWindowAttribute(hackWindow, DWMWA_CAPTION_BUTTON_BOUNDS, &captionAreaRect, sizeof(RECT));

    int controlWidth = captionAreaRect.right - captionAreaRect.left;
    int controlHeight = captionAreaRect.bottom - captionAreaRect.top;
    int srcBorderTop = 1;
    int dstWindowWidth = windowRect.right - windowRect.left;
    int dstX = clientRect.right - controlWidth;
    int frameWidth = GetSystemMetrics(SM_CXFIXEDFRAME);

    // Carve out the area for custom content
    windowRect.left = windowRect.right - controlWidth;
    windowRect.bottom = windowRect.top + 20;
    HRGN minRgn = CreateRectRgnIndirect(&windowRect);
    CombineRgn(hRgn, hRgn, minRgn, RGN_XOR);
    DeleteObject(minRgn);

    // Force default painting for non-client area
    LRESULT ret = DefWindowProc(hWnd, WM_NCPAINT, (WPARAM)hRgn, 0);
    
    // copy bitmaps from other window- may work after reading
    // https://stackoverflow.com/questions/242570/copying-content-from-a-hidden-or-clipped-window-in-xp
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd183402(v=vs.85).aspx
    ShowWindow(hackWindow, SW_SHOWNORMAL);
    RECT hackWindowRect;
    GetWindowRect(hackWindow, &hackWindowRect);

    int srcWindowWidth = hackWindowRect.right - hackWindowRect.left;

    HDC hDC = GetWindowDC(hWnd);
    HDC hdcWindow = GetWindowDC(hackWindow);

    BitBlt(hDC,
      dstX, 0,
      controlWidth, controlHeight,
      hdcWindow,
      (srcWindowWidth - controlWidth - frameWidth), srcBorderTop,
      SRCCOPY);

    ReleaseDC(hWnd, hdcWindow);

    ShowWindow(hackWindow, SW_HIDE);


    //// Calls I discovered via:
    //// http://stackoverflow.com/questions/34004819/windows-10-close-minimize-and-maximize-buttons
    //// Reference:
    //// https://msdn.microsoft.com/en-us/library/windows/desktop/bb773289(v=vs.85).aspx
    //SetWindowTheme(hWnd, L"EXPLORER", NULL);
    //HTHEME hTheme = OpenThemeData(hWnd, L"WINDOW");

    //// This code is bad and I feel like a horrible person
    //int partState = active
    //  ? buttons[TBB_MINIMIZE].pressed
    //    ? MINBS_PUSHED
    //    : buttons[TBB_MINIMIZE].hovering
    //      ? MINBS_HOT
    //      : MINBS_NORMAL
    //  : buttons[TBB_MINIMIZE].hovering
    //    ? MINBS_HOT
    //    : MINBS_DISABLED;
    //DrawThemeBackground(hTheme, hDC, WP_MINBUTTON, partState, &buttons[TBB_MINIMIZE].clientRect, NULL);

    //partState = maximized
    //  ? active
    //    ? buttons[TBB_MAXIMIZE].pressed
    //      ? RBS_PUSHED
    //      : buttons[TBB_MAXIMIZE].hovering
    //        ? RBS_HOT
    //        : RBS_NORMAL
    //    : buttons[TBB_MAXIMIZE].hovering
    //      ? RBS_HOT
    //      : RBS_DISABLED
    //: active
    //  ? buttons[TBB_MAXIMIZE].pressed
    //    ? MAXBS_PUSHED
    //    : buttons[TBB_MAXIMIZE].hovering
    //      ? MAXBS_HOT
    //      : MAXBS_NORMAL
    //  : buttons[TBB_MAXIMIZE].hovering
    //    ? MAXBS_HOT
    //    : MAXBS_DISABLED;
    //DrawThemeBackground(hTheme, hDC, maximized ? WP_RESTOREBUTTON : WP_MAXBUTTON, partState, &buttons[TBB_MAXIMIZE].clientRect, NULL);

    //partState = active
    //  ? buttons[TBB_CLOSE].pressed
    //    ? CBS_PUSHED
    //    : buttons[TBB_CLOSE].hovering
    //      ? CBS_HOT
    //      : CBS_NORMAL
    //  : buttons[TBB_CLOSE].hovering
    //    ? CBS_HOT
    //    : CBS_DISABLED;
    //DrawThemeBackground(hTheme, hDC, WP_CLOSEBUTTON, partState, &buttons[TBB_CLOSE].clientRect, NULL);

    //CloseThemeData(hTheme);

    ReleaseDC(hWnd, hDC);

    if (wParam == 1) {
      DeleteObject(hRgn);
    }

    return ret;
  }

  return DefWindowProc(hWnd, WM_NCPAINT, wParam, lParam);
}
