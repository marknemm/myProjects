#include "DataOptionsh.h"


DataOptions::ScrollEdit::ScrollEdit(){}


void DataOptions::ScrollEdit::initialize(MorphWind *parent)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "The argument *parent is NULL");
	}
	RECT dim = { 195, 478, 225, 507 };
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_NUMBER), dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origEditProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
}


bool DataOptions::ScrollEdit::isInputValid()
{
	char text[5] = "";
	sendMessage(WM_GETTEXT, 5, (LPARAM)text);
	_numScrolls = std::atoi(text);
	return (_numScrolls != 0);
}


int DataOptions::ScrollEdit::getNumScrolls() const
{
	return _numScrolls;
}


LRESULT CALLBACK DataOptions::ScrollEdit::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_CHAR:
			if((wParam == 13) && isInputValid()) {
				getParent().sendMessage((WM_APP + SET_SCROLLS_BUTTON_ID), NULL, NULL);
				return 0;
			}
			break;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}