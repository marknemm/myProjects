#include "NumOfScrollsMenu.h"


NumOfScrollsMenu::NumOfScrollsMenu(int prevNumScrolls)
: _isWindowAlive(true),
  _goBack(false),
  _cwnd()
{
	int tens = (prevNumScrolls / 10);
	int ones = (prevNumScrolls % 10);
	std::stringstream ss;
	if(tens > 0) {
		ss << tens;
	}
	ss << ones;
    
	RECT dimensions;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dimensions);
	dimensions.left = ((dimensions.right / 2) - 170);
	dimensions.top = (dimensions.bottom / 5);
	dimensions.right = (dimensions.left + 350);
	dimensions.bottom = (dimensions.top + 150);
	initialize("Number of scrolls Menu", "Number of scrolls Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dimensions, NULL);
	RECT dimC = { 200, 65, 230, 95 };
	_cwnd.initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE), dimC, this);
	SetWindowLongPtr(_cwnd.getHWND(), GWLP_USERDATA, LONG(&_cwnd));
	_cwnd._origEditProc = WNDPROC(SetWindowLongPtr(_cwnd.getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, 
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,  VARIABLE_PITCH, TEXT("Times New Roman"));
	_cwnd.sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	if(prevNumScrolls == 0) {
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM("1"));
	}
	else {
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM(ss.str().c_str()));
	}
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);
    
    showWindow(SW_SHOWNORMAL);
	setForeground();
	update();
	_cwnd.showWindow(SW_SHOWNORMAL);
	_cwnd.update();

	_cwnd.sendMessage(EM_SETSEL, 0, MAKELONG(0, 1));
	_cwnd.setFocus();
}


NumOfScrollsMenu::ChildNumOfScrollsMenu::ChildNumOfScrollsMenu()
: _numScrolls(0),
  _ctrlDown(false),
  _entered(false)
{}


int NumOfScrollsMenu::waitForInput()
{
	MSG msg;
	while(_isWindowAlive && GetMessage(&msg, NULL, 0, 0)) {
		if(msg.hwnd == getHWND() || msg.hwnd == _cwnd.getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return _cwnd._numScrolls;
}

bool NumOfScrollsMenu::wasInputEntered() const
{
	return _cwnd._entered;
}

bool NumOfScrollsMenu::wasBackEntered() const
{
	return _goBack;
}

bool NumOfScrollsMenu::ChildNumOfScrollsMenu::isInputValid()
{
	char text[7] = "";
	sendMessage(WM_GETTEXT, 7, LPARAM(text));
	_numScrolls = std::atoi(text);
	return (_numScrolls != 0);
}

LRESULT CALLBACK NumOfScrollsMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_LBUTTONDOWN:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if((y > 61) && (y < 93)) {
					RECT rcClient;
					GetClientRect(hwnd, &rcClient);
					if((x > 10) && (x < 30)) {
						_cwnd._entered = true;
						_goBack = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
					else if((x > (rcClient.right - 30)) && (x < (rcClient.right-10)) && (_cwnd.isInputValid())) {
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
				Gdiplus::RectF clientRectF;
				clientRectF.X = Gdiplus::REAL(rcClient.left);
				clientRectF.Y = Gdiplus::REAL(rcClient.top);
				clientRectF.Width = Gdiplus::REAL(rcClient.right - rcClient.left);
				clientRectF.Height = Gdiplus::REAL(rcClient.bottom - rcClient.top);
				HDC hdcBuffer = CreateCompatibleDC(hdc);
				HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
				SelectObject(hdcBuffer, hBitmap);
				Gdiplus::Graphics graphics(hdcBuffer);
				Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom), Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
				graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
				RECT textRect = { 100 ,  50 ,  200 ,  130 };
				Gdiplus::SolidBrush sb(Gdiplus::Color(0, 0, 0));
				Gdiplus::Font font(L"Arial", 25);
				Gdiplus::Font font2(L"Arial", 16);
				Gdiplus::StringFormat stringForm;
				stringForm.SetAlignment(Gdiplus::StringAlignmentCenter);
				graphics.DrawString(L"How many scrolls would you like to perform at the input location?", 65, &font2, clientRectF, &stringForm, &sb);
				graphics.DrawString(L"Scrolls:", 8, &font, Gdiplus::PointF(Gdiplus::REAL(80), Gdiplus::REAL(60)), NULL, &sb);
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
			showWindow( SW_RESTORE);
			_cwnd.setForeground();
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


LRESULT CALLBACK NumOfScrollsMenu::ChildNumOfScrollsMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			else if((wParam == 8) || (wParam == 127)) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
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