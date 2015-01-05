#include "FinishedMenu.h"


FinishedMenu::FinishedMenu()
: _ctrlDown(false),
  _goBack(false),
  _entered(false),
  _highlightNo(false),
  _highlightYes(false),
  _isWindowActive(true),
  yesRect(Gdiplus::RectF(Gdiplus::REAL(35), Gdiplus::REAL(73), Gdiplus::REAL(175), Gdiplus::REAL(40))),
  noRect(Gdiplus::RectF(Gdiplus::REAL(240), Gdiplus::REAL(73), Gdiplus::REAL(175), Gdiplus::REAL(40)))

{
	initYesRect();
	initNoRect();
    
	RECT dimensions;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dimensions);
	dimensions.left = ((dimensions.right / 2) - 232);
	dimensions.top = (dimensions.bottom / 5);
	dimensions.right = (dimensions.left + 465);
	dimensions.bottom = (dimensions.top + 170);
	initialize("Finished Menu", "Finished Menu", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dimensions, NULL);
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);

	showWindow(SW_SHOWNORMAL);
	update();
	setFocus();
}


void FinishedMenu::initYesRect()
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


void FinishedMenu::initNoRect()
{
	Gdiplus::RectF noRect(noRect);
	Gdiplus::GraphicsPath *gPath = new Gdiplus::GraphicsPath();
	gPath = new Gdiplus::GraphicsPath();
	gPath->AddRectangle(noRect);
	int numClrs(1);
	noPgb = new Gdiplus::PathGradientBrush(gPath);
	Gdiplus::Color *surClr = new Gdiplus::Color(200, 0, 0);
	noPgb->SetCenterColor(Gdiplus::Color(255, 200, 200));
	noPgb->SetSurroundColors(surClr,  &numClrs);
	sbBlack = new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
	sbDGray = new Gdiplus::SolidBrush(Gdiplus::Color(200, 10, 10, 10));
	sbHlght = new Gdiplus::SolidBrush(Gdiplus::Color(200, 100, 100, 150));
	delete gPath, surClr;
}


bool FinishedMenu::waitForDecision()
{
	MSG msg;
	while(_isWindowActive && GetMessage(&msg, nullptr, 0, 0)) {
		if(msg.hwnd == getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return _decision;
}


bool FinishedMenu::wasInputEntered()
{
	return _entered;
}


bool FinishedMenu::wasBackEntered() const
{
	return _goBack;
}


LRESULT CALLBACK FinishedMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			} else if(_highlightNo || _highlightYes) {
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
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			} else if((x >= noRect.GetLeft()) && (x <= noRect.GetRight()) && (y >= noRect.GetTop()) && (y <= noRect.GetBottom())) {
				_entered = true;
				_decision = false;
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			} else if((x > 4) && (y > 76) && (x < 26) && (y < 108)) {
				_entered = true;
				_goBack = true;
				sendMessage(WM_CLOSE, 0, 0);
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
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			case VK_CONTROL:
				_ctrlDown = true;
				break;
			case VK_BACK:
				if(_ctrlDown) {
					_entered = true;
					_goBack = true;
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				}
				break;
			case VK_OEM_MINUS:
				showWindow(SW_MINIMIZE);
				break;
			}
		break;
	case WM_KEYUP:
		if(wParam == VK_CONTROL) {
			_ctrlDown = true;
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
			Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom), Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
			graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
			Gdiplus::Font font(L"Arial", 19);
			GetClientRect(hwnd, &rcClient);
			Gdiplus::StringFormat sform;
			sform.SetAlignment(Gdiplus::StringAlignmentCenter);
			Gdiplus::RectF clientRectF(Gdiplus::REAL(rcClient.left), Gdiplus::REAL(rcClient.top), Gdiplus::REAL(rcClient.right - rcClient.left), Gdiplus::REAL(rcClient.bottom - rcClient.top));
			graphics.DrawString(L"Would you like to continue adding to the new record?", 52, &font, clientRectF, &sform, sbBlack);
			Gdiplus::Font fontB(L"Arial", 22);
			graphics.FillRectangle(yesPgb, yesRect);
			graphics.FillRectangle(noPgb, noRect);
			yesRect.Y = (noRect.Y += 2);
			if(!_highlightYes) {
				graphics.DrawString(L"Continue", 8, &fontB, yesRect, &sform, sbDGray);
			}
			else {
				graphics.DrawString(L"Continue", 8, &fontB, yesRect, &sform, sbHlght);
			}
			if(!_highlightNo) {
				graphics.DrawString(L"Finished", 8, &fontB, noRect, &sform, sbDGray);
			}
			else {
				graphics.DrawString(L"Finished", 8, &fontB, noRect, &sform, sbHlght);
			}
			yesRect.Y = (noRect.Y -= 2);
			graphics.DrawImage(&Gdiplus::Image(L"BackSymbolPop.png", 0), 5, 77);

			RECT rcClip;
			GetClipBox(hdc, &rcClip);
			BitBlt(hdc, rcClip.left, -rcClip.top, (rcClip.right - rcClip.left), (rcClip.bottom + rcClip.top), hdcBuffer, rcClip.left, -rcClip.top, SRCCOPY);
			DeleteObject(hBitmap);
			DeleteObject(hdcBuffer);
			EndPaint(hwnd, &ps);
		}
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_HOTKEY:
		showWindow(SW_RESTORE);
		setForeground();
		setFocus();
		break;
	case WM_CLOSE:
		UnregisterHotKey(hwnd, 057);
		break;
	case WM_NCDESTROY:
		_isWindowActive = false;
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}