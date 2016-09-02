#pragma once
#include <windows.h>
#include <windowsx.h>

typedef enum titlebarButtonTypeEnum {
  TBB_MINIMIZE,
  TBB_MAXIMIZE,
  TBB_CLOSE
} titlebarButtonType;

typedef enum titlebarButtonStateEnum {
  TBBS_REGULAR,
  TBBS_HOVER,
  TBBS_PRESSED
} titlebarButtonState;

class titlebarButton {
public:
  int commandId;
  RECT clientRect;
  RECT screenRect;
  bool mouseDown;
  bool pressed;
  bool hovering;
};
