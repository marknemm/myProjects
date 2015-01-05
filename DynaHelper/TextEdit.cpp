#include "DataOptionsh.h"


DataOptions::TextEdit::TextEdit(){}


void DataOptions::TextEdit::initialize(MorphWind *parent)
{
	RECT dim = { 195, 433, 735, 462 };
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED), dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origEditProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
}


bool DataOptions::TextEdit::isTextValid()
{
	char text[128] = "";
	sendMessage(WM_GETTEXT, 128, (LPARAM)text);
	_input = text;
	return (_input.compare("") != 0);
}


std::string& DataOptions::TextEdit::getText()
{
	return _input;
}


LRESULT CALLBACK DataOptions::TextEdit::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_CHAR:
			if((wParam == 13) && isTextValid()) {
				getParent().sendMessage((WM_APP + SET_TEXT_BUTTON_ID), NULL, NULL);
				return 0;
			}
			break;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}