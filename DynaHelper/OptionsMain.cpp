#include"OptionsMain.h"


OptionsMain::OptionsMain()
: _dataOpts()
{
	HWND desktop = GetDesktopWindow();
	RECT desktopDim;
	GetClientRect(desktop, &desktopDim);
	RECT dim = { ((desktopDim.right / 2) - 450), ((desktopDim.bottom / 2) - 375), 900, 675 };
	dim.right += dim.left;
	dim.bottom += dim.top;
	initialize("Options Main", "Options Main", _globHINST, WS_EX_APPWINDOW, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dim, NULL);
	_dataOpts.initialize(this);
	showWindow(SW_SHOWNORMAL);
	update();
	setForeground();
	_dataOpts.setFocus();
	run();
}


void OptionsMain::run()
{
	MSG msg;
	while(isWindowVisible() && GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


void OptionsMain::paint()
{
	PAINTSTRUCT ps;	HDC hdc;
	hdc = BeginPaint(getHWND(), &ps);
	RECT rcClient;
	getClientDim(&rcClient);
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
	SelectObject(hdcBuffer, hBitmap);
	Gdiplus::Graphics graphics(hdcBuffer);
	BitBlt(hdc, 0, 0, 900, 600, hdcBuffer, 0, 0, SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(hdcBuffer);
	EndPaint(getHWND(), &ps);
}


void OptionsMain::mouseMove(LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
}


void OptionsMain::leftButtonDown(LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
}


bool OptionsMain::close()
{
	if(!_dataOpts.wasChangeMade() && _dataOpts.exitCmd() == CMD_EXIT) {
		return true;
	}
	switch(_dataOpts.exitCmd()) {
		case CMD_EXIT_AND_SAVE:
			return (MessageBox(getHWND(), "Are you sure you want to SAVE AND EXIT?", "Save And Exit Confirmation", (MB_YESNO | MB_ICONQUESTION)) == IDYES);
		case CMD_EXIT:
			return (MessageBox(getHWND(), "Are you sure you want to EXIT WITHOUT SAVING?", "Exit Confirmation", (MB_YESNO | MB_ICONQUESTION)) == IDYES);
		default:
			return false;
	}
}


LRESULT CALLBACK OptionsMain::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_PAINT:			paint();					break;
		case WM_MOUSEMOVE:		mouseMove(lParam);			break;
		case WM_LBUTTONDOWN:	leftButtonDown(lParam);		break;
		case WM_CLOSE:
			if(!close()) {  return 0;  }
			break;
		case WM_DESTROY:
			if(_dataOpts.exitCmd() == CMD_EXIT_AND_SAVE) {
				_recordHandler->save();
			}
			break;
		case WM_ERASEBKGND:									return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}