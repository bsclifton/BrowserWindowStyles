#include <Windows.h>

#define ID_FILE_QUIT 11
#define ID_FILE_NEW_TAB 1

RECT menuRect;

void AppendFileMenu(HMENU parentMenu);
void AppendEditMenu(HMENU parentMenu);
void AppendHistoryMenu(HMENU parentMenu);
void AppendBookmarksMenu(HMENU parentMenu);
void AppendBraveryMenu(HMENU parentMenu);
void AppendHelpMenu(HMENU parentMenu);

//Menu functions
//https://msdn.microsoft.com/en-us/library/windows/desktop/ff468865(v=vs.85).aspx

void AddMenus(HMENU parentMenu) {
  AppendFileMenu(parentMenu);
  AppendEditMenu(parentMenu);
  AppendMenu(parentMenu, NULL, NULL, L"View");
  AppendHistoryMenu(parentMenu);
  AppendBookmarksMenu(parentMenu);
  AppendBraveryMenu(parentMenu);
  AppendMenu(parentMenu, NULL, NULL, L"Window");
  AppendHelpMenu(parentMenu);
}

LRESULT OnMenu(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  switch (LOWORD(wParam)) {
    case ID_FILE_NEW_TAB: {
      MessageBeep(250);
    } return NULL;
    case ID_FILE_QUIT:
      PostQuitMessage(0);
      return NULL;
  }

  return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
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
  AppendMenu(editMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(editMenu, MF_STRING, NULL, L"Cut");
  AppendMenu(editMenu, MF_STRING, NULL, L"Copy");
  AppendMenu(editMenu, MF_STRING, NULL, L"Paste");
  AppendMenu(editMenu, MF_STRING, NULL, L"Paste without formatting");
  AppendMenu(editMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(editMenu, MF_STRING, NULL, L"Delete");
  AppendMenu(editMenu, MF_STRING, NULL, L"Select All");
  AppendMenu(editMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(editMenu, MF_STRING, NULL, L"Find on Page...");
  AppendMenu(editMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(editMenu, MF_STRING, NULL, L"Settings...");

  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)editMenu, L"Edit");
}

void AppendHistoryMenu(HMENU parentMenu) {
  HMENU historyMenu = CreateMenu();

  AppendMenu(historyMenu, MF_STRING, NULL, L"Home");
  AppendMenu(historyMenu, MF_STRING, NULL, L"Back");
  AppendMenu(historyMenu, MF_STRING, NULL, L"Forward");

  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)historyMenu, L"History");
}

void AppendBookmarksMenu(HMENU parentMenu) {
  HMENU bookmarksMenu = CreateMenu();

  AppendMenu(bookmarksMenu, MF_STRING, NULL, L"Bookmark Page");
  AppendMenu(bookmarksMenu, MF_SEPARATOR, NULL, NULL);
  AppendMenu(bookmarksMenu, MF_STRING, NULL, L"Bookmarks Manager...");

  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)bookmarksMenu, L"Bookmarks");
}

void AppendBraveryMenu(HMENU parentMenu) {
  HMENU braveryMenu = CreateMenu();

  AppendMenu(braveryMenu, MF_STRING, NULL, L"Bravery global settings...");
  AppendMenu(braveryMenu, MF_STRING, NULL, L"Bravery site settings...");

  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)braveryMenu, L"Bravery");
}

void AppendHelpMenu(HMENU parentMenu) {
  HMENU helpMenu = CreateMenu();

  AppendMenu(helpMenu, MF_STRING, NULL, L"Report an Issue");
  AppendMenu(helpMenu, MF_STRING, NULL, L"Submit Feedback...");
  AppendMenu(helpMenu, MF_STRING, NULL, L"Spread the word about Brave...");
  AppendMenu(helpMenu, MF_STRING, NULL, L"Check for Updates");
  AppendMenu(helpMenu, MF_STRING, NULL, L"About Brave");

  AppendMenu(parentMenu, MF_STRING | MF_POPUP, (UINT)helpMenu, L"Help");
}
