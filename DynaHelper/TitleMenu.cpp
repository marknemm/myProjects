#include "TitleMenu.h"


TitleMenu::ChildTitleMenu::ChildTitleMenu()
: _title(""),
  _entered(false),
  _ctrlDown(false)
{}


TitleMenu::TitleMenu(int randomize, const std::string &prevTitle)
: _goBack(false),
  _isWindowAlive(true),
  _cwnd()
{
	RECT dimensions;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dimensions);
	dimensions.left = ((dimensions.right / 2) - 175);
	dimensions.top = (dimensions.bottom / 5);
	dimensions.right = (dimensions.left + 350);
	dimensions.bottom = (dimensions.top + 130);
	initialize("Title Menu", "Title Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dimensions, NULL);

	RECT dimC = { 116, 45, 286, 75 };
	_cwnd.initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE), dimC, this);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	_cwnd.sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	SetWindowLongPtr(_cwnd.getHWND(), GWLP_USERDATA, LONG(&_cwnd));
	_cwnd._origEditProc = WNDPROC(SetWindowLongPtr(_cwnd.getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));

	if(prevTitle == "") {
		srand(size_t(time(NULL)) + randomize);
		int randm = rand() % 899999 + 100000;
		std::stringstream ss;
		ss << "Record " << randm;
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM(ss.str().c_str()));
	}
	else {
		_cwnd.sendMessage(WM_SETTEXT, 0, LPARAM(prevTitle.c_str()));
	}
	_cwnd.sendMessage(EM_SETSEL, 0, MAKELONG(0, 12));
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);
    
    showWindow(SW_SHOWNORMAL);
	update();
}


std::string TitleMenu::waitForTitle()
{
	MSG msg;
	_cwnd.setFocus();
	while(_isWindowAlive && GetMessage(&msg, nullptr, 0, 0)) {
		if(msg.hwnd == getHWND() || msg.hwnd == _cwnd.getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return _cwnd._title;
}


bool TitleMenu::wasTitleEntered() const
{
	return _cwnd._entered;
}


bool TitleMenu::wasBackEntered() const
{
	return _goBack;
}


bool TitleMenu::ChildTitleMenu::isInputValid()
{
	char text[30] = "";
	sendMessage(WM_GETTEXT, 30, LPARAM(text));
	_title = text;
	return (_title.compare("") != 0);
}


LRESULT CALLBACK TitleMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(wParam == VK_CONTROL) {
				_cwnd._ctrlDown = true;
			}
			else if(wParam == VK_OEM_MINUS) {
				showWindow(SW_MINIMIZE);
			}
			else if((wParam == VK_RETURN) && _cwnd.isInputValid()) {
				_cwnd._entered = true;
				postMessage(WM_CLOSE, 0, 0);
			}
			else if(wParam == VK_ESCAPE) {
				postMessage(WM_CLOSE, 0, 0);
			}
			else if(wParam == VK_BACK) {
				if(_cwnd._ctrlDown) {
					_cwnd._entered = true;
					_goBack = true;
					postMessage(WM_CLOSE, 0, 0);
				}
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
		case WM_LBUTTONDOWN:
			{
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if((y > 43) && (y < 75)) {
					if((x > 4) && (x < 26)) {
						_cwnd._entered = true;
						_goBack = true;
						postMessage(WM_CLOSE, 0, 0);
					}
					else if((x > (rcClient.right - 26)) && (x < (rcClient.right - 4)) && _cwnd.isInputValid()) {
						_cwnd._entered = true;
						postMessage(WM_CLOSE, 0, 0);
					}
				}
			}
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				HDC hdcBuffer = CreateCompatibleDC(hdc);
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
				SelectObject(hdcBuffer, hBitmap);
				Gdiplus::Graphics graphics(hdcBuffer);
				Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom)
				  , Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
				graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
				RECT textRect = { 100 ,  50 ,  200 ,  130 };
				Gdiplus::SolidBrush sb(Gdiplus::Color(255, 0, 0, 0));
				Gdiplus::Font font(L"Arial", 25);
				Gdiplus::Font font2(L"Arial", 16);
				graphics.DrawString(L"Enter a title for your new record.", 34, &font2, Gdiplus::PointF(10, 7), NULL, &sb);
				graphics.DrawString(L"Title: ", 7, &font, Gdiplus::PointF(Gdiplus::REAL(36), Gdiplus::REAL(40)), NULL, &sb);
				graphics.DrawImage(&Gdiplus::Image(L"BackSymbolPop.png", 0), 5, 44);
				graphics.DrawImage(&Gdiplus::Image(L"ForwardSymbolPop.png", 0), rcClient.right-25, 44);
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
		case WM_DESTROY:
			_isWindowAlive = false;
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


LRESULT CALLBACK TitleMenu::ChildTitleMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(wParam == VK_CONTROL) {
				_ctrlDown = true;
				getParent().setFocus();
			}
			break;
		case WM_CHAR:
			if(((wParam >= 'A') && (wParam <= 'Z')) || ((wParam >= 'a') && (wParam <= 'z')) || ((wParam >= '0') && (wParam <= '9')) || (wParam == ' ') || (wParam == '_')) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if(wParam == 27) {
				getParent().postMessage(WM_CLOSE, 0, 0);
			}
			else if((wParam == 8) || (wParam == 127)) {
				CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
			}
			else if((wParam == 13) && isInputValid()) {
				_entered = true;
				getParent().postMessage(WM_CLOSE, 0, 0);
			}
			return 0;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}