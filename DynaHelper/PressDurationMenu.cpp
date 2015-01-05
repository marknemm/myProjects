#include "PressDurationMenu.h"


PressDurationMenu::PressDurationMenu(double prevDuration)
: _isWindowAlive(true),
  _goBack(false),
  _cwnd()
{
	int thousands = int(prevDuration / 1000);
	int hundreds = int((prevDuration - (thousands * 1000)) / 100);
	int tens = int((prevDuration - (thousands * 1000) - (hundreds * 100)) / 10);
	int ones = (int(prevDuration) % 10);
	int tenths = int(std::abs(prevDuration - int(prevDuration)) * 10);
	int hundredths = (int(std::abs(prevDuration - int(prevDuration)) * 100) - (tenths*10));
	int thousandths = (int(std::abs(prevDuration - int(prevDuration)) * 1000) - (hundredths*10) - (tenths*100));
	std::stringstream ss;
	if(thousands > 0) {
		ss << thousands;
	}
	if((hundreds > 0) || (thousands > 0)) {
		ss << hundreds;
	}
	if((tens > 0) || (hundreds > 0) || (thousands > 0)) {
		ss << tens;
	}
	ss << ones;
	if((tenths > 0) || (hundredths > 0) || (thousandths > 0)) {
		ss << '.' << tenths;
		if((hundredths > 0) || (thousandths > 0)) {
			ss << hundredths;
			if(thousandths > 0) {
				ss << thousandths;
			}
		}
	}
    
	RECT dimensions;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dimensions);
	dimensions.left = ((dimensions.right / 2) - 170);
	dimensions.top = (dimensions.bottom / 5);
	dimensions.right = (dimensions.left + 350);
	dimensions.bottom = (dimensions.top + 130);
	initialize("Press Duration Menu", "Press Duration Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dimensions, NULL);
	RECT dimC = { 165, 45, 236, 75 };
	_cwnd.initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE), dimC, this);
	SetWindowLongPtr(_cwnd.getHWND(), GWLP_USERDATA, LONG(&_cwnd));
	_cwnd._origEditProc = WNDPROC(SetWindowLong(_cwnd.getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	_cwnd.sendMessage(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	if(prevDuration == 0.0) {
		SetWindowText(_cwnd.getHWND(), "1.5");	
	}
	else {
		SetWindowText(_cwnd.getHWND(), ss.str().c_str());
	}
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);
    
    showWindow(SW_SHOWNORMAL);
	update();
	setForeground();
	_cwnd.showWindow(SW_SHOWNORMAL);
	_cwnd.update();

	_cwnd.sendMessage(EM_SETSEL, 0, MAKELONG(0, 3));
	_cwnd.setFocus();
}


PressDurationMenu::ChildPressDurationMenu::ChildPressDurationMenu()
: _ctrlDown(false),
  _entered(false),
  _durations(0.0)
{}


int PressDurationMenu::waitForDuration()
{
	MSG msg;
	int delaysec;
	while(_isWindowAlive && GetMessage(&msg, NULL, 0, 0)) {
		if(msg.hwnd == getHWND() || msg.hwnd == _cwnd.getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	delaysec = int(_cwnd._durations * 1000);
	return delaysec;
}

bool PressDurationMenu::wasDurationEntered() const
{
	return _cwnd._entered;
}

bool PressDurationMenu::wasBackEntered() const
{
	return _goBack;
}

bool PressDurationMenu::ChildPressDurationMenu::isInputValid()
{
	char text[7] = "";
	sendMessage(WM_GETTEXT, 7, LPARAM(text));
	_durations = std::atof(text);
	return (_durations != 0.0);
}

LRESULT CALLBACK PressDurationMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_LBUTTONDOWN:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if((y > 44) && (y < 76)) {
					RECT rcClient;
					getClientDim(&rcClient);
					if((x > 4) && (x < 26)) {
						_cwnd._entered = true;
						_goBack = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
					else if((x > (rcClient.right - 26)) && (x < (rcClient.right-4)) && _cwnd.isInputValid()) {
						_cwnd._entered = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
				}
			}
			break;
		case WM_KEYDOWN:
			if((wParam == VK_CONTROL) || (wParam == VK_BACK)) {
				if(wParam == VK_CONTROL) {
					_cwnd._ctrlDown = true;
				}
				_goBack = (_cwnd._ctrlDown && (wParam == VK_BACK));
				if(_goBack) {
					_cwnd._entered = true;
					postMessage(WM_CLOSE, 0, 0);
				}
			}
			else if(wParam == VK_OEM_MINUS) {
				showWindow(SW_MINIMIZE);
			}
			else if(wParam == VK_ESCAPE) {
				sendMessage(WM_CLOSE, 0, 0);
			}
			else if((wParam == VK_RETURN) && _cwnd.isInputValid()) {
				_cwnd._entered = true;
				sendMessage(WM_CLOSE, 0, 0);
			}
			else {
				_cwnd.postMessage(message, wParam, lParam);
				_cwnd.setFocus();
			}
			break;
		case WM_KEYUP:
			if(wParam == VK_CONTROL) {
				_cwnd._ctrlDown = false;
			}
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				HDC hdcBuffer = CreateCompatibleDC(hdc);
				HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
				SelectObject(hdcBuffer, hBitmap);
				Gdiplus::Graphics graphics(hdcBuffer);
				Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom), Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
				graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
				RECT textRect = { 100 ,  50 ,  200 ,  130 };
				Gdiplus::SolidBrush sb(Gdiplus::Color(0, 0, 0));
				Gdiplus::Font font(L"Ariel", 25);
				Gdiplus::Font font2(L"Ariel", 16);
				graphics.DrawString(L"How long should the press occur?", 32, &font2, Gdiplus::PointF(2, 5), NULL, &sb);
				graphics.DrawString(L"Duration:", 9, &font, Gdiplus::PointF(Gdiplus::REAL(22), Gdiplus::REAL(40)), NULL, &sb);
				graphics.DrawString(L"sec.", 4, &font, Gdiplus::PointF(Gdiplus::REAL(242), Gdiplus::REAL(40)), NULL, &sb);
				graphics.DrawImage(&Gdiplus::Image(L"BackSymbolPop.png", 0), 5, 45);
				graphics.DrawImage(&Gdiplus::Image(L"ForwardSymbolPop.png", 0), (rcClient.right - 25), 45);
				RECT rcClip;
				GetClipBox(hdc, &rcClip);
				BitBlt(hdc, rcClip.left, -rcClip.top, (rcClip.right - rcClip.left), (rcClip.bottom + rcClip.top), hdcBuffer, rcClip.left, -rcClip.top, SRCCOPY);
				DeleteObject(hBitmap);
				DeleteObject(hdcBuffer);
				EndPaint(hwnd, &ps);
			}
			break;
		case WM_HOTKEY:
			showWindow(SW_RESTORE);
			setForeground();
			_cwnd.setFocus();
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_CLOSE:
			UnregisterHotKey(hwnd, 057);
			break;
		case WM_NCDESTROY:
			_isWindowAlive = false;
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK PressDurationMenu::ChildPressDurationMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(wParam == VK_CONTROL) {
				_ctrlDown = true;
				getParent().setFocus();
			}
			break;
		case WM_CHAR:
			if((wParam >= '0') && (wParam <= '9')) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if(wParam == '.') {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if((wParam == 8) || (wParam == 127)) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if(wParam == 27) {
				getParent().postMessage(WM_CLOSE, 0, 0);
			}
			else if((wParam == 13) && (isInputValid())) {
				_entered = true;
				getParent().postMessage(WM_CLOSE, 0, 0);
			}
			return 0;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}