#include "DataOptionsh.h"


DataOptions::DelayEdit::DelayEdit()
: _delay(0.0),
  _lowerDelayOk(false),
  _buttonID(SET_DELAY_BUTTON_ID)
{}


void DataOptions::DelayEdit::initialize(MorphWind *parent, bool isDuration)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "The argument *parent is NULL");
	}
	RECT dim = { 685, 343, 816, 372 };
	if(isDuration) {
		dim.top = 388;
		dim.bottom = 417;
		_buttonID = SET_DURATION_BUTTON_ID;
	}
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED), dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origEditProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
}


bool DataOptions::DelayEdit::isInputValid()
{
	char text[7] = "";
	sendMessage(WM_GETTEXT, 7, LPARAM(text));
	_delay = std::atof(text);
	return (_delay != 0.0);
}


int DataOptions::DelayEdit::getDelay()
{
	int delaysec = int(_delay * 1000);
	if((delaysec < 700) && !_lowerDelayOk && _buttonID != SET_DURATION_BUTTON_ID) {
		delaysec = 700;
	}
	return delaysec;
}


LRESULT CALLBACK DataOptions::DelayEdit::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_CHAR:
			if(((wParam >= '0') && (wParam <= '9')) || (wParam == '.') || (wParam == 8) || (wParam == 127)) {
				break;
			}
			else if((wParam == 13) && isInputValid()) {
				getParent().sendMessage((WM_APP + _buttonID), NULL, NULL);
			}
			return 0;
		case WM_SETTEXT:
			_lowerDelayOk = bool(wParam);
			break;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}