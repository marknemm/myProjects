#include "KeyInputVerificationMenu.h"


KeyInputVerificationMenu::KeyInputVerificationMenu(int keyCode)
: _goBack(false),
  _ctrlDown(false),
  _entered(false),
  _highlightNo(false),
  _highlightYes(false),
  _isWindowAlive(true),
  yesRect(Gdiplus::RectF(Gdiplus::REAL(35), Gdiplus::REAL(73), Gdiplus::REAL(170), Gdiplus::REAL(40))),
  noRect(Gdiplus::RectF(Gdiplus::REAL(245), Gdiplus::REAL(73), Gdiplus::REAL(170), Gdiplus::REAL(40)))
{
	initYesRect();
	initNoRect();
	sbBlack = new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
	sbDGray = new Gdiplus::SolidBrush(Gdiplus::Color(200, 10, 10, 10));
	sbHlght = new Gdiplus::SolidBrush(Gdiplus::Color(200, 100, 100, 150));
    
	RECT dim;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dim);
	dim.left = ((dim.right / 2) - 232);
	dim.top = (dim.bottom / 5);
	dim.right = (dim.left + 465);
	dim.bottom = (dim.top + 170);
	initialize("Key Input Verification Menu", "Key Input Verification Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dim, NULL);

	displayText = L"Did you want to use the ";
	if((keyCode > 0x40) && (keyCode < 0x5B)) {
		displayText += char(keyCode);
	}
	else if((keyCode > 0x2F) && (keyCode < 0x3A)) {
		displayText += char(keyCode);
	}
	else {
		switch(keyCode) {
			case VK_BACK:			displayText+=L"BACKSPACE";		break;
			case VK_TAB:			displayText+=L"TAB";			break;
			case VK_CLEAR:			displayText+=L"CLEAR";			break;
			case VK_RETURN:			displayText+=L"ENTER";			break;
			case VK_PAUSE:			displayText+=L"PAUSE";			break;
			case VK_CAPITAL:		displayText+=L"CAPS LOCK";		break;
			case VK_ESCAPE:			displayText+=L"ESC";			break;
			case VK_SPACE:			displayText+=L"SPACE";			break;
			case VK_PRIOR:			displayText+=L"PG UP";			break;
			case VK_NEXT:			displayText+=L"PG DN";			break;
			case VK_END:			displayText+=L"END";			break;
			case VK_HOME:			displayText+=L"HOME";			break;
			case VK_LEFT:			displayText+=L"LEFT ARROW";		break;
			case VK_UP:				displayText+=L"UP ARROW";		break;
			case VK_RIGHT:			displayText+=L"RIGHT ARROW";	break;
			case VK_DOWN:			displayText+=L"DOWN ARROW";		break;
			case VK_SELECT:			displayText+=L"SELECT";			break;
			case VK_PRINT:			displayText+=L"PRINT";			break;
			case VK_EXECUTE:		displayText+=L"EXECUTE";		break;
			case VK_SNAPSHOT:		displayText+=L"PRT SC";			break;
			case VK_INSERT:			displayText+=L"INS";			break;
			case VK_DELETE:			displayText+=L"DELETE";			break;
			case VK_HELP:			displayText+=L"HELP";			break;
			case VK_SLEEP:			displayText+=L"SLEEP";			break;
			case VK_NUMPAD0:		displayText+=L"NUM PAD 0";		break;
			case VK_NUMPAD1:		displayText+=L"NUM PAD 1";		break;
			case VK_NUMPAD2:		displayText+=L"NUM PAD 2";		break;
			case VK_NUMPAD3:		displayText+=L"NUM PAD 3";		break;
			case VK_NUMPAD4:		displayText+=L"NUM PAD 4";		break;
			case VK_NUMPAD5:		displayText+=L"NUM PAD 5";		break;
			case VK_NUMPAD6:		displayText+=L"NUM PAD 6";		break;
			case VK_NUMPAD7:		displayText+=L"NUM PAD 7";		break;
			case VK_NUMPAD8:		displayText+=L"NUM PAD 8";		break;
			case VK_NUMPAD9:		displayText+=L"NUM PAD 9";		break;
			case VK_MULTIPLY:		displayText+=L"MULTIPLY(*)";	break;
			case VK_ADD:			displayText+=L"PLUS(+)";		break;
			case VK_SEPARATOR:		displayText+=L"COLON(:)";		break;
			case VK_SUBTRACT:		displayText+=L"SUBTRACT(-)";	break;
			case VK_DECIMAL:		displayText+=L"DECIMAL(.)";		break;
			case VK_DIVIDE:			displayText+=L"DIVIDE(/)";		break;
			case VK_F1:				displayText+=L"F1";				break;
			case VK_F2:				displayText+=L"F2";				break;
			case VK_F3:				displayText+=L"F3";				break;
			case VK_F4:				displayText+=L"F4";				break;
			case VK_F5:				displayText+=L"F5";				break;
			case VK_F6:				displayText+=L"F6";				break;
			case VK_F7:				displayText+=L"F7";				break;
			case VK_F8:				displayText+=L"F8";				break;
			case VK_F9:				displayText+=L"F9";				break;
			case VK_F10:			displayText+=L"F10";			break;
			case VK_F11:			displayText+=L"F11";			break;
			case VK_F12:			displayText+=L"F12";			break;
			case VK_NUMLOCK:		displayText+=L"NUM LOCK";		break;
			case VK_SCROLL:			displayText+=L"SCROLL LOCK";	break;
			case VK_LSHIFT:			displayText+=L"LEFT SHIFT";		break;
			case VK_RSHIFT:			displayText+=L"RIGHT SHIFT";	break;
			case VK_LCONTROL:		displayText+=L"LEFT CTRL";		break;
			case VK_RCONTROL:		displayText+=L"RIGHT CTRL";		break;
			case VK_LMENU:			displayText+=L"LEFT ALT";		break;
			case VK_RMENU:			displayText+=L"RIGHT ALT";		break;
			case VK_VOLUME_MUTE:	displayText+=L"VOLUME MUTE";	break;
			case VK_VOLUME_DOWN:	displayText+=L"VOLUME DOWN";	break;
			case VK_VOLUME_UP:		displayText+=L"VOLUEM UP";		break;
			case VK_ZOOM:			displayText+=L"ZOOM";			break;
			case VK_SHIFT:			displayText+=L"SHIFT";			break;
			case VK_CONTROL:		displayText+=L"CTRL";			break;
			case VK_MENU:			displayText+=L"ALT";			break;
			case VK_OEM_1:			displayText+=L"; (or :)";		break;
			case VK_OEM_2:			displayText+=L"/ (or ?)";		break;
			case VK_OEM_3:			displayText+=L"` (or ~)";		break;
			case VK_OEM_4:			displayText+=L"[ (or {)";		break;
			case VK_OEM_5:			displayText+=L"\\ (or |)";		break;
			case VK_OEM_6:			displayText+=L"] (or })";		break;
			case VK_OEM_7:			displayText+=L"' (or QUOTE)";	break;
			case VK_OEM_COMMA:		displayText+=L",  (or <)";		break;
			case VK_OEM_PERIOD:		displayText+=L". (or >)";		break;
			case VK_OEM_PLUS:		displayText+=L"+ (or =)";		break;
		}
	}
	displayText += L" key in this recording?";
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);

	RECT newDim;
	setDim(HWND_TOPMOST, newDim, (SWP_NOMOVE | SWP_NOSIZE));
	showWindow(SW_SHOWNORMAL);
	setForeground();
	setFocus();
	update();
}


void KeyInputVerificationMenu::initYesRect()
{
	Gdiplus::RectF yesRect(yesRect);
	Gdiplus::GraphicsPath *gPath = new Gdiplus::GraphicsPath();
	gPath->AddRectangle(yesRect);
	int numClrs(1);
	yesPgb = new Gdiplus::PathGradientBrush(gPath);
	Gdiplus::Color *surClr = new Gdiplus::Color(0, 200, 0);
	yesPgb->SetCenterColor(Gdiplus::Color(200, 255, 200));
	yesPgb->SetSurroundColors(surClr, &numClrs);
	delete gPath, surClr;
}


void KeyInputVerificationMenu::initNoRect()
{
	Gdiplus::RectF noRect(noRect);
	Gdiplus::GraphicsPath *gPath = new Gdiplus::GraphicsPath();
	gPath = new Gdiplus::GraphicsPath();
	gPath->AddRectangle(noRect);
	int numClrs(1);
	noPgb = new Gdiplus::PathGradientBrush(gPath);
	Gdiplus::Color *surClr = new Gdiplus::Color(200, 0, 0);
	noPgb->SetCenterColor(Gdiplus::Color(255, 200, 200));
	noPgb->SetSurroundColors(surClr, &numClrs);
	delete gPath, surClr;
}


bool KeyInputVerificationMenu::waitForDecision()
{
	MSG msg;
	_isWindowAlive = true;
	while(_isWindowAlive && GetMessage(&msg, NULL, 0, 0)) {
		if(msg.hwnd == getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return _decision;
}


bool KeyInputVerificationMenu::wasInputEntered() const
{
	return _entered;
}


bool KeyInputVerificationMenu::wasBackEntered() const
{
	return _goBack;
}


LRESULT CALLBACK KeyInputVerificationMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
	case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			if((x >= yesRect.GetLeft()) && (x <= yesRect.GetRight()) && (y >= yesRect.GetTop()) && (y <= yesRect.GetBottom())) {
				if(!_highlightYes) {
					_highlightYes = true;
					_highlightNo = false;
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				}
			}
			else if((x >= noRect.GetLeft()) && (x <= noRect.GetRight()) && (y >= noRect.GetTop()) && (y <= noRect.GetBottom())) {
				if(!_highlightNo) {
					_highlightNo = true;
					_highlightYes = false;
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				}
			}
			else if(_highlightNo || _highlightYes) {
				_highlightNo = false;
				_highlightYes = false;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			if((x >= yesRect.GetLeft()) && (x <= yesRect.GetRight()) && (y >= yesRect.GetTop()) && (y <= yesRect.GetBottom())) {
				_entered = true;
				_decision = true;
				sendMessage(WM_CLOSE, 0, 0);
			}
			else if((x >= noRect.GetLeft()) && (x <= noRect.GetRight()) && (y >= noRect.GetTop()) && (y <= noRect.GetBottom())) {
				_entered = true;
				_decision = false;
				sendMessage(WM_CLOSE, 0, 0);
			}
			else if((y > 75) && (y < 107)) {
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				if((x > 4) && (x < 26)) {
					_decision = false;
					_entered = true;
					_goBack = true;
					sendMessage(WM_CLOSE, 0, 0);
				}
				else if((x > (rcClient.right - 26)) && (x < (rcClient.right-4))) {
					_entered = true;
					_decision = true;
					sendMessage(WM_CLOSE, 0, 0);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		switch(wParam) {
		case VK_RETURN:
			if(!_highlightNo) {
				_entered = true;
				_decision = true;
				sendMessage(WM_CLOSE, 0, 0);
			}
			else {
				_entered = true;
				_decision = false;
				sendMessage(WM_CLOSE, 0, 0);
			}
			break;
		case 0x59: //Y key
			_entered = true;
			_decision = true;
			sendMessage(WM_CLOSE, 0, 0);
			break;
		case 0x4E: //N key
			_entered = true;
			_decision = false;
			sendMessage(WM_CLOSE, 0, 0);
			break;
		case VK_TAB:
			if(!_highlightYes) {
				_highlightYes = true;
				_highlightNo = false;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			else {
				_highlightNo = true;
				_highlightYes = false;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		case VK_ESCAPE:
			_entered = false;
			_goBack = false;
			sendMessage(WM_CLOSE, 0, 0);
			break;
		case VK_CONTROL:
			_ctrlDown = true;
			break;
		case VK_BACK:
			if(_ctrlDown) {
				_entered = true;
				_goBack = true;
				sendMessage(WM_CLOSE, 0, 0);
			}
			break;
		case VK_OEM_MINUS:
			showWindow(SW_MINIMIZE);
			break;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HDC hdcBuffer = CreateCompatibleDC(hdc);
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			clientRect.X = Gdiplus::REAL(rcClient.left);
			clientRect.Y = Gdiplus::REAL(rcClient.top);
			clientRect.Width = Gdiplus::REAL(rcClient.right - rcClient.left);
			clientRect.Height = Gdiplus::REAL(rcClient.bottom - rcClient.top);
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
			SelectObject(hdcBuffer, hBitmap);
			Gdiplus::Graphics graphics(hdcBuffer);
			Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom), Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
			graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
			Gdiplus::StringFormat strForm;
			strForm.SetAlignment(Gdiplus::StringAlignmentCenter);
			Gdiplus::Font font(L"Arial", 19);
			Gdiplus::Font fontB(L"Arial", 22);
			graphics.DrawString(displayText.c_str(), int(displayText.size()), &font, clientRect, &strForm, sbBlack);
			graphics.FillRectangle(yesPgb, yesRect);
			graphics.FillRectangle(noPgb, noRect);
			yesRect.Y += 3;
			noRect.Y += 3;
			if(!_highlightYes) {
				graphics.DrawString(L"Right Key", 9, &fontB, yesRect, &strForm, sbDGray);
			}
			else {
				graphics.DrawString(L"Right Key", 9, &fontB, yesRect, &strForm, sbHlght);
			}
			if(!_highlightNo) {
				graphics.DrawString(L"Wrong Key", 9, &fontB, noRect, &strForm, sbDGray);
			}
			else {
				graphics.DrawString(L"Wrong Key", 9, &fontB, noRect, &strForm, sbHlght);
			}
			yesRect.Y -= 3;
			noRect.Y -= 3;
			graphics.DrawImage(&Gdiplus::Image(L"BackSymbolPop.png", 0), 5, 76);
			graphics.DrawImage(&Gdiplus::Image(L"ForwardSymbolPop.png", 0), (rcClient.right - 25), 76);
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
		setFocus();
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