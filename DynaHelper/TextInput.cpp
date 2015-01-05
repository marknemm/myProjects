#include "TextInput.h"
#include <sstream>


TextInput::ChildTextInput::ChildTextInput()
: _ctrlDown(false),
  _entered(false),
  _input("")
{}


TextInput::TextInput()
: _isWindowAlive(true),
  _goBack(false),
  _cwnd()
{}


TextInput::TextInput(const std::string &prevString)
: _isWindowAlive(true),
  _goBack(false),
  _cwnd()
{
	RECT dimensions;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dimensions);

	RECT dim = { ((dimensions.right / 2) - 350), (dimensions.bottom / 5), 700, 120 };
	dim.right += dim.left;
	dim.bottom += dim.top;
	initialize("Text Input Menu", "Text Input Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dim, NULL);
	
	RECT dimC = { 110, 25, 651, 55 };
	_cwnd.initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE), dimC, this);
	SetWindowLongPtr(_cwnd.getHWND(), GWLP_USERDATA, LONG(&_cwnd));
	_cwnd._origEditProc = WNDPROC(SetWindowLongPtr(_cwnd.getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	_cwnd.sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	if(prevString.size() > 0) {
		SetWindowText(_cwnd.getHWND(), prevString.c_str());
		_cwnd.sendMessage(EM_SETSEL, 0, MAKELONG(0, prevString.size()));
	}
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);

    showWindow(SW_SHOWNORMAL);
	update();
	_cwnd.showWindow(SW_SHOWNORMAL);
	_cwnd.update();
	_cwnd.setFocus();
}


std::string TextInput::waitForInput()
{
	MSG msg;
	while(_isWindowAlive && GetMessage(&msg, NULL, 0, 0)) {
		if(msg.hwnd == getHWND() || msg.hwnd == _cwnd.getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return _cwnd._input;
}


bool TextInput::wasInputEntered() const
{
	return _cwnd._entered;
}


bool TextInput::wasBackEntered() const
{
	return _goBack;
}


bool TextInput::ChildTextInput::isInputValid()
{
	char text[128] = "";
	sendMessage(WM_GETTEXT, 128, (LPARAM)text);
	_input = text;
	return (_input.compare("") != 0);
}


LRESULT CALLBACK TextInput::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_LBUTTONDOWN:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if((y > 24) && (y < 56)) {
					RECT rcClient;
					getClientDim(&rcClient);
					if((x > 10) && (x < 30)) {
						_cwnd._entered = true;
						_goBack = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
					else if((x > (rcClient.right - 30)) && (x < (rcClient.right-10)) && _cwnd.isInputValid()) {
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
				RECT textRect = { 100 , 50 , 200 , 130 };
				Gdiplus::SolidBrush sb(Gdiplus::Color(255, 0, 0, 0));
				Gdiplus::Font font(L"Arial", 25);
				graphics.DrawString(L"Text:", 5, &font, Gdiplus::PointF(Gdiplus::REAL(25), Gdiplus::REAL(20)), NULL, &sb);
				graphics.DrawImage(&Gdiplus::Image(L"BackSymbolPop.png", 0), 10, 24);
				graphics.DrawImage(&Gdiplus::Image(L"ForwardSymbolPop.png", 0), (rcClient.right - 30), 25);
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


LRESULT CALLBACK TextInput::ChildTextInput::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(wParam == VK_CONTROL) {
				_ctrlDown = true;
				getParent().setFocus();
			}
			break;
		case WM_CHAR:
			if(wParam == 27) {
				getParent().postMessage(WM_CLOSE, 0, 0);
				return 0;
			}
			else if((wParam == 13) && isInputValid()) {
				_entered = true;
				getParent().postMessage(WM_CLOSE, 0, 0);
				return 0;
			}
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}