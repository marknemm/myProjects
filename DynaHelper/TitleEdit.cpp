#include "DataOptionsh.h"


DataOptions::TitleEdit::TitleEdit()
: _title("")
{}


void DataOptions::TitleEdit::initialize(MorphWind *parent)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "The argument *parent is NULL");
	}
	RECT dim = { 685, 297, 855, 326 };
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED), dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origEditProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
}


bool DataOptions::TitleEdit::isInputValid()
{
	char text[30] = "";
	sendMessage(WM_GETTEXT, 30, LPARAM(text));
	_title = text;
	return (_title.compare("") != 0);
}


std::string& DataOptions::TitleEdit::getTitle()
{
	return _title;
}


LRESULT CALLBACK DataOptions::TitleEdit::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_CHAR:
			if(((wParam >= 'A') && (wParam <= 'Z')) || ((wParam >= 'a') && (wParam <= 'z')) || ((wParam >= '0') && (wParam <= '9')) || (wParam == ' ') || (wParam == '_')) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if((wParam == 8) || (wParam == 127)) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if((wParam == 13) && isInputValid()) {
				getParent().sendMessage((WM_APP + SET_TITLE_BUTTON_ID), NULL, NULL);
			}
			return 0;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}