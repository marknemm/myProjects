#include "RecordScreen.h"


RecordScreen::RecordScreen(bool paintBackSymbol)
: _pressed(false),
  _didMouseMove(false),
  _setToWork(false),
  _returnable(false),
  _returnOn(false),
  _drawTrajectoryLine(false),
  _goBack(false),
  _paintBackSymbol(paintBackSymbol),
  _keyPressed(0),
  _location(),
  _location2(),
  _pt1(),
  _pt2(),
  _displayText(L""),
  _action(DEFAULT_ACT),
  _resolution(),
  _textBoundBox(),
  _backRect(Gdiplus::REAL(115), Gdiplus::REAL(75), Gdiplus::REAL(0), Gdiplus::REAL(75))
{
	RECT dim;
	GetWindowRect(GetDesktopWindow(), &dim);
	_resolution.X = Gdiplus::REAL(dim.left);
	_resolution.Y = Gdiplus::REAL(dim.top + ((dim.bottom + dim.top) / 2));
	_resolution.Width = Gdiplus::REAL(dim.right - dim.left);
	_resolution.Height = Gdiplus::REAL(dim.bottom - dim.top);
	dim.left = -12;
	dim.top = -70;
	dim.right = int(dim.right + 12);
	dim.bottom = int(dim.bottom + 45);
	MorphWind::initialize("Record Screen", "", _globHINST, WS_EX_LAYERED, WS_POPUPWINDOW, dim, NULL);
	_prevStyle = SetWindowLong(getHWND(), GWL_STYLE, NULL);
	SetLayeredWindowAttributes(getHWND(), RGB(0, 0, 0), 75, LWA_ALPHA);
	showWindow(SW_SHOWNORMAL);
	setForeground();
	setFocus();
	update();
}


/*void RecordScreen::initialize(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, MorphWind *parent)
{
	RECT dim;
	GetWindowRect(GetDesktopWindow(), &dim);
	_resolution.X = Gdiplus::REAL(dim.left);
	_resolution.Y = Gdiplus::REAL(dim.top);
	_resolution.Width = Gdiplus::REAL(dim.right - dim.left);
	_resolution.Height = Gdiplus::REAL(dim.bottom - dim.top);
	dim.left = -12;
	dim.top = -70;
	dim.right = int(_resolution.Width + 12);
	dim.bottom = int(_resolution.Height + 45);
	MorphWind::initialize(className, name, hInstance, exStyle, style, dim, parent);
}*/


void RecordScreen::waitForInput(const int action)
{
	MSG msg;
	setScreenEnabled(action);
	if(GetFocus() != getHWND())
		setFocus();
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);
	boost::thread returnTimerThread(&RecordScreen::returnTimer, this);
	while(_run && GetMessage(&msg, NULL, 0, 0)) {
		if(msg.hwnd == getHWND()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


bool RecordScreen::wasBackEntered() const
{
	return _goBack;
}


int RecordScreen::keyEntered() const
{
	return _keyPressed;
}


Point RecordScreen::locationEntered()
{
	return _location;
}


Point RecordScreen::location2Entered()
{
	return _location2;
}


void RecordScreen::returnTimer() {
	int goCount(0);
	while(_run) {
		//For turning window on w/o prior input coming through:
		if(_setToWork) {
			if(goCount > 4) {
				_returnable = true;
				_setToWork = false;
			}
			goCount++;
		}
		//else {  goCount = 0;  }
		//For counting down hovering over return button:
		if(_paintBackSymbol) {
			if((_returnOn) && (_backRect.Width < 96)) {
				_backRect.X -= 2;
				_backRect.Width += 2;
				RedrawWindow(getHWND(), NULL, NULL, RDW_INVALIDATE);
			}
			else if((!_returnOn) && (_backRect.Width > 0)) {
				_backRect.X += 2;
				_backRect.Width -= 2;
				RedrawWindow(getHWND(), NULL, NULL, RDW_INVALIDATE);
			}
		}
		Sleep(25);
	}
}


void RecordScreen::setScreenEnabled(const int action)
{
	RECT displayRect;
	_setToWork = true;
	_run = true;
	_action = action;
	HDC hdc = GetDC(getHWND());
	Gdiplus::Graphics graphics(hdc);
	if(action & SECOND_POINT)
		_displayText = L"CLICK AND DRAG ACROSS THE RECORDING SCREEN IN ANY AREA";
	else if(action & FIRST_POINT)
		_displayText = L"CLICK THE RECORDING SCREEN OR HIT ANY KEY AT A SPECIFIC LOCATION";
	else {  _displayText = L"TYPE A DESIRED KEY FOR THE SELECTED KEY EVENT";  }
	graphics.MeasureString(_displayText.c_str(), _displayText.length(), &Gdiplus::Font(L"Ariel", 50), _resolution, &_textBoundBox);
	ReleaseDC(getHWND(), hdc);
	Gdiplus::REAL xOff((_resolution.Width - _textBoundBox.Width) / 2);
	Gdiplus::REAL yOff((_resolution.Height - _textBoundBox.Height) / 2);
	_textBoundBox.X = xOff;
	_textBoundBox.Y = yOff;
	displayRect.left = LONG(_textBoundBox.X);
	displayRect.top = LONG(_textBoundBox.Y);
	displayRect.right = LONG(_textBoundBox.GetRight());
	displayRect.bottom = LONG(_textBoundBox.GetBottom());
	redrawWindow(&displayRect, NULL, (RDW_INVALIDATE | RDW_UPDATENOW));
}


void RecordScreen::setLayeredWindowAttributes(COLORREF colorRef, BYTE bAlpha, DWORD dwFlags)
{
	SetLayeredWindowAttributes(getHWND(), colorRef, bAlpha, dwFlags);
}


void RecordScreen::updateNow()
{
	RedrawWindow(getHWND(), NULL, NULL, (RDW_INVALIDATE | RDW_UPDATENOW));
}


void RecordScreen::mouseMove(LPARAM lParam)
{
	if(_returnable) {
		int x(GET_X_LPARAM(lParam));
		int y(GET_Y_LPARAM(lParam));
		SYSTEMTIME st;
		GetSystemTime(&st);
		int newMS((st.wHour * 60 * 60 * 1000) + (st.wMinute * 60 * 1000) + (st.wSecond * 1000) + st.wMilliseconds);
		if((_drawTrajectoryLine) && ((newMS - _startMS) > 33)) {
			HRGN updateRgn = CreateRectRgn((_pt2.x - 8), (_pt2.y - 8), (_pt2.x + 8), (_pt2.y + 8));
			_pt2.x = GET_X_LPARAM(lParam);
			_pt2.y = GET_Y_LPARAM(lParam);
			HRGN updateRgn2 = CreateRectRgn((_pt2.x - 8), (_pt2.y - 8), (_pt2.x + 8), (_pt2.y + 8));
			CombineRgn(updateRgn, updateRgn, updateRgn2, RGN_OR);
			redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
			_startMS = newMS;
			_didMouseMove = true;
		}
		else if((x > 14) && (x < 116) && (y > 74) && (y < 151) && (!_pressed))
			_returnOn = true;
		else {  _returnOn = false;  }
	}
}


void RecordScreen::leftButtonDown(LPARAM lParam)
{
	if(_backRect.Width > 95) {
		_returnOn = false;
		_returnable = false;
		_goBack = true;
		sendMessage(WM_CLOSE, 0, 0);
	}
	else if(_returnable && (_action & FIRST_POINT)) {
		Point point;
		POINT pos;
		GetCursorPos(&pos);
		point.x = pos.x;
		point.y = pos.y;
		_location = point;
		if(!(_action & SECOND_POINT))
			sendMessage(WM_CLOSE, 0, 0);
		else {
			_pt1.x = GET_X_LPARAM(lParam);
			_pt1.y = GET_Y_LPARAM(lParam);
			_pressed = true;
			_drawTrajectoryLine = true;
			SYSTEMTIME st;
			GetSystemTime(&st);
			_startMS = ((st.wHour * 60 * 60 * 1000) + (st.wMinute * 60 * 1000) + (st.wSecond * 1000) + st.wMilliseconds);
			HRGN updateRgn = CreateRectRgn((_pt1.x - 8), (_pt1.y - 8), (_pt1.x + 8), (_pt1.y + 8));
			redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
		}
	}
}


void RecordScreen::leftButtonUp(LPARAM lParam)
{
	if(_returnable && _pressed) {
		Point point;
		if(!_didMouseMove) {
			_pt2.x = _pt1.x;
			_pt2.y = _pt1.y;
		}
		if(_action & SECOND_POINT) {
			POINT pos;
			GetCursorPos(&pos);
			point.x = pos.x;
			point.y = pos.y;
		}
		else {
			point.x = -1;
			point.y = -1;
		}
		_location2 = point;
		sendMessage(WM_CLOSE, 0, 0);
	}
}


void RecordScreen::keyDown(WPARAM wParam)
{
	if(_returnable) {
		Point point;
		if(wParam == VK_OEM_MINUS)
			showWindow(SW_MINIMIZE);
		else if(!(_action & FIRST_POINT)) {
			point.x = 0;
			point.y = 0;
			_location = point;
			_keyPressed = wParam;
			sendMessage(WM_CLOSE, 0, 0);
		}
		else if(!(_action & SECOND_POINT)) {
			POINT curPos;
			GetCursorPos(&curPos);
			point.x = curPos.x;
			point.y = curPos.y;
			_location = point;
			_keyPressed = wParam;
			sendMessage(WM_CLOSE, 0, 0);
		}
	}
}


void RecordScreen::drawTrajectoryLine(Gdiplus::Graphics &graphics, RECT &rcClip)
{
	if(!(rcClip.right < (_pt1.x - 8) || rcClip.left > (_pt1.x + 8) || rcClip.bottom < (_pt1.y - 8) || rcClip.top > (_pt1.y + 8)))
		graphics.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 255, 0)), (_pt1.x - 7 - rcClip.left), (_pt1.y - 7 - rcClip.top), 14, 14);
	if(!(rcClip.right < (_pt2.x - 8) || rcClip.left > (_pt2.x + 8) || rcClip.bottom < (_pt2.y - 8) || rcClip.top > (_pt2.y + 8)))
		graphics.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 0, 0)), (_pt2.x - 7 - rcClip.left), (_pt2.y - 7 - rcClip.top), 14, 14);
}


void RecordScreen::paint()
{
	using namespace Gdiplus;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(getHWND(), &ps);
	RECT rcClip;
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	GetClipBox(hdc, &rcClip);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
	SelectObject(hdcBuffer, hBitmap);
	Graphics graphics(hdcBuffer);

	SolidBrush background(Color(255, 255, 255));
	graphics.FillRectangle(&background, 0, 0, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
	
	if(!(_textBoundBox.GetRight() < rcClip.left || _textBoundBox.X > rcClip.right || _textBoundBox.GetBottom() < rcClip.top || _textBoundBox.Y > rcClip.bottom)) {
		SolidBrush tb(Color(50, 50, 255));
		StringFormat strForm;
		strForm.SetAlignment(StringAlignmentCenter);
		Font font(L"Arial", 50);
		RectF textBoundBox(REAL(_textBoundBox.X - rcClip.left), REAL(_textBoundBox.Y - rcClip.top), _textBoundBox.Width, _textBoundBox.Height);
		graphics.DrawString(_displayText.c_str(), (INT)_displayText.size(), &font, textBoundBox, &strForm, &tb);
	}

	if(_paintBackSymbol) {
		graphics.DrawImage(&Image(L"BackSymbol.png", FALSE), (20 - rcClip.left), (75 - rcClip.top));
		if(!(rcClip.right < _backRect.X || rcClip.left > _backRect.GetRight() || rcClip.bottom < _backRect.Y || rcClip.top > _backRect.GetBottom())) {
			if(_backRect.Width < 96) {
				graphics.FillRectangle(&SolidBrush(Color(40, 250, 115, 0)), _backRect);
				if(_drawTrajectoryLine)
					drawTrajectoryLine(graphics, rcClip);
			}
			else {  graphics.FillRectangle(&SolidBrush(Color(40, 150, 150, 255)), _backRect);  }
		}
		else {  drawTrajectoryLine(graphics, rcClip);  }
	}
	else if(_drawTrajectoryLine)
		drawTrajectoryLine(graphics, rcClip);
	
	BitBlt(hdc, rcClip.left, rcClip.top, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top), hdcBuffer, 0, 0, SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(hdcBuffer);
	EndPaint(getHWND(), &ps);
}


void RecordScreen::hotKey() // ctrl + w
{
	if(_run)
		showWindow(SW_RESTORE);
}


void RecordScreen::getMinMaxInfo(UINT message, WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(getHWND(), message, wParam, lParam); // Probably don't need this.... don't even know why it's here...
	MINMAXINFO *mmi = (MINMAXINFO*)lParam;
	RECT _resolution;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &_resolution);
	mmi->ptMaxTrackSize.x = (_resolution.right + 25);
	mmi->ptMaxTrackSize.y = (_resolution.bottom + 115);
}


void RecordScreen::close()
{
	SetWindowLong(getHWND(), GWL_STYLE, _prevStyle);
	UnregisterHotKey(getHWND(), 057);
}


LRESULT CALLBACK RecordScreen::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_MOUSEMOVE:			mouseMove(lParam);							break;
		case WM_LBUTTONDOWN:		leftButtonDown(lParam);						break;
		case WM_LBUTTONUP:			leftButtonUp(lParam);						break;
		case WM_KEYDOWN:			keyDown(wParam);							break;
		case WM_PAINT:				paint();									break;
		case WM_HOTKEY:				hotKey();									break;
		case WM_CLOSE:				close();									break;
		case WM_NCDESTROY:			_run = false;								break;
		case WM_GETMINMAXINFO:		getMinMaxInfo(message, wParam, lParam);		return 0;
		case WM_ERASEBKGND:														return 0;
		case WM_MOUSEACTIVATE:													return MA_NOACTIVATE;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}