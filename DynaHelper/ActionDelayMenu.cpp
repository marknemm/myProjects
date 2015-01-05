#include "ActionDelayMenu.h"


ActionDelayMenu::ActionDelayMenu()
: _goBack(false),
  _isWindowOpen(true),
  _cwnd()
{}


ChildActionDelayMenu::ChildActionDelayMenu()
: _ctrlDown(false),
  _entered(false),
  _delayms(0.0)
{}


ActionDelayMenu::ActionDelayMenu(bool lowerDelayOk, double prevDelay)
: _goBack(false),
  _isWindowOpen(true),
  _cwnd()
{
	int thousands = int(prevDelay / 1000);
	int hundreds = int((prevDelay - (thousands * 1000)) / 100);
	int tens = int((prevDelay - (thousands * 1000) - (hundreds * 100)) / 10);
	int ones = (int(prevDelay) % 10);
	int tenths = int(std::abs(prevDelay - int(prevDelay)) * 10);
	int hundredths = (int(std::abs(prevDelay - int(prevDelay)) * 100) - (tenths*10));
	int thousandths = (int(std::abs(prevDelay - int(prevDelay)) * 1000) - (hundredths*10) - (tenths*100));
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

	RECT dim = { ((dimensions.right / 2) - 170), (dimensions.bottom / 5), 350, 150 };
	dim.right += dim.left;
	dim.bottom += dim.top;
	initialize("Action Delay Menu", "Action Delay Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dim, NULL);

	RECT dimC = { 146, 65, 217, 95 };
	_cwnd.initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE), dimC, this);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	_cwnd.sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	SetWindowLongPtr(_cwnd.getHWND(), GWLP_USERDATA, LONG(&_cwnd));
	_cwnd._origEditProc = WNDPROC(SetWindowLongPtr(_cwnd.getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
	
	if(prevDelay > 0.0) {
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM(ss.str().c_str()));
	}
	else if(!lowerDelayOk) {
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM("2"));
	}
	else {
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM("0.001"));
	}
	_cwnd.sendMessage(EM_SETSEL, 0, MAKELONG(0, 5));
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);	//ctrl + w
    
    showWindow(SW_SHOWNORMAL);
	update();
}


int ActionDelayMenu::waitForDelay(bool lowerDelayOk) {
	int delaysec;
	MSG msg;
	_cwnd.setFocus();
	while (_isWindowOpen && GetMessage(&msg, nullptr, 0, 0)) {
		if(msg.hwnd == getHWND() || msg.hwnd == _cwnd.getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	/*for(BOOL m1, m2 = FALSE ; _isWindowOpen ; WaitMessage()) {
		m1 = peekMessage(&msg, 0, 0, PM_REMOVE);
		m2 = _cwnd.peekMessage(&msgB, 0, 0, PM_REMOVE);
		while(m1 || m2) {
			if(msg.message == WM_CLOSE) {
				postMessage(WM_CLOSE, 0, 0);
			}
			if(m1) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if(m2) {
				TranslateMessage(&msgB);
				DispatchMessage(&msgB);
			}
			m1 = peekMessage(&msg, 0, 0, PM_REMOVE);
			m2 = _cwnd.peekMessage(&msgB, 0, 0, PM_REMOVE);
		}
	}*/
	delaysec = int(_cwnd._delayms * 1000);
	if((delaysec < 700) && !lowerDelayOk) {
		delaysec = 700;
	}
	return delaysec;
}


bool ActionDelayMenu::wasDelayEntered() const
{
	return _cwnd._entered;
}


bool ActionDelayMenu::wasBackEntered() const
{
	return _goBack;
}


bool ChildActionDelayMenu::isInputValid()
{
	char text[7] = "";
	sendMessage(WM_GETTEXT, 7, LPARAM(text));
	_delayms = std::atof(text);
	return (_delayms != 0.0);
}


LRESULT CALLBACK ActionDelayMenu::wndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message) {
		case WM_LBUTTONDOWN:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if((y > 61) && (y < 93)) {
					RECT rcClient;
					getClientDim(&rcClient);
					if((x > 10) && (x < 30)) {
						_cwnd._entered = true;
						_goBack = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
					else if((x > (rcClient.right - 30)) && (x < (rcClient.right - 10)) && (_cwnd.isInputValid())) {
						_cwnd._entered = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
				}
			}
			break;
		case WM_SIZE:
			if(wParam == SIZE_RESTORED) {
				setForeground();
				_cwnd.setFocus();
			}
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
				SendMessage(hwnd,WM_CLOSE,0,0);
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
				Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom)
				  , Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
				graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
				RECT textRect = { 100, 50, 200 , 130 };
				Gdiplus::SolidBrush sb(Gdiplus::Color(0, 0, 0));
				Gdiplus::Font font(L"Arial", 25);
				Gdiplus::Font font2(L"Arial", 16);
				graphics.DrawString(L"How long of a delay should occur", 32, &font2, Gdiplus::PointF(4, 5), NULL, &sb);
				graphics.DrawString(L"before the action is executed?", 30, &font2, Gdiplus::PointF(20, 28), NULL, &sb);
				graphics.DrawString(L"Delay: ", 7, &font, Gdiplus::PointF(Gdiplus::REAL(41), Gdiplus::REAL(60)), NULL, &sb);
				graphics.DrawString(L"sec.", 4, &font, Gdiplus::PointF(Gdiplus::REAL(223), Gdiplus::REAL(60)), NULL, &sb);
				graphics.DrawImage(&Gdiplus::Image(L"BackSymbolPop.png", 0), 10, 62);
				graphics.DrawImage(&Gdiplus::Image(L"ForwardSymbolPop.png", 0), (rcClient.right - 30), 62);
	
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
			update();
			_cwnd.setForeground();
			SetWindowPos(getHWND(), HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
			_cwnd.setFocus();
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_CLOSE:
			SetWindowLongPtr(_cwnd.getHWND(), GWLP_WNDPROC, LONG(_cwnd._origEditProc));
			UnregisterHotKey(hwnd, 057);
			break;
		case WM_DESTROY:
			_isWindowOpen = false;
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


LRESULT CALLBACK ChildActionDelayMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(wParam == VK_CONTROL) {
				_ctrlDown = true;
				getParent().setFocus();
			}
		case WM_CHAR:
			if(((wParam >= '0') && (wParam <= '9')) || (wParam == '.') || (wParam == 8) || (wParam == 127)) {
				break;
			}
			else if(wParam == 27) {
				getParent().postMessage(WM_CLOSE, 0, 0);
			}
			else if((wParam == 13) && isInputValid()) {
				_entered = true;
				getParent().postMessage(WM_CLOSE, 0, 0);
			}
			return 0;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}