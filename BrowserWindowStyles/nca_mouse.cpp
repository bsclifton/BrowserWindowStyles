#include "main.h"

extern void RedrawNC(HWND hwnd);
extern titlebarButton buttons[3];
extern UINT buttonCommandIds[3];

LRESULT OnLButtonDownNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);
  POINT point = { GET_X_LPARAM(lParam) - windowRect.left, GET_Y_LPARAM(lParam) - windowRect.top };

  for (int i = TBB_MINIMIZE; i <= TBB_CLOSE; i++) {
    if (PtInRect(&buttons[i].clientRect, point)) {
      buttons[i].mouseDown = true;
      buttons[i].pressed = true;
      //SetCapture(hWnd);
      RedrawNC(hWnd);
      return 0;
    } else {
      buttons[i].mouseDown = false;
      buttons[i].pressed = false;
    }
  }

  return DefWindowProc(hWnd, WM_NCLBUTTONDOWN, wParam, lParam);
}

LRESULT OnLButtonUpNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);
  POINT point = { GET_X_LPARAM(lParam) - windowRect.left, GET_Y_LPARAM(lParam) - windowRect.top };

  for (int i = TBB_MINIMIZE; i <= TBB_CLOSE; i++) {
    if (buttons[i].mouseDown) {
      //ReleaseCapture();
      if (PtInRect(&buttons[i].clientRect, point)) {
        SendMessage(hWnd, WM_COMMAND, buttonCommandIds[i], MAKELPARAM(point.x, point.y));
      }
      buttons[i].mouseDown = false;
      buttons[i].pressed = false;
      RedrawNC(hWnd);
      return 0;
    }
  }

  return DefWindowProc(hWnd, WM_LBUTTONUP, wParam, lParam);
}

LRESULT OnMouseMoveNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);
  POINT point = { GET_X_LPARAM(lParam) - windowRect.left, GET_Y_LPARAM(lParam) - windowRect.top };
  bool redraw = false;

  // is mouse hovering over control
  for (int i = TBB_MINIMIZE; i <= TBB_CLOSE; i++) {
    bool hovering = PtInRect(&buttons[i].clientRect, point) ? true : false;

    if (hovering != buttons[i].hovering) {
      buttons[i].hovering = !buttons[i].hovering;
      redraw = true;
      break;
    }
  }

  // is mouse pressing button (or did it move away)
  for (int i = TBB_MINIMIZE; i <= TBB_CLOSE; i++) {
    if (buttons[i].mouseDown) {
      bool pressed = PtInRect(&buttons[i].clientRect, point) ? true : false;

      if (pressed != buttons[i].pressed) {
        buttons[i].pressed = !buttons[i].pressed;
        redraw = true;
      }

      break;
    }
  }

  if (redraw) {
    RedrawNC(hWnd);
  }

  return DefWindowProc(hWnd, WM_NCMOUSEMOVE, wParam, lParam);
}
