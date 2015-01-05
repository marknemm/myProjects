#include"ActionOffsetMenu.h"


ActionOffsetMenu::ActionOffsetMenu(int prevAction)
: _goBack(false),
  _ctrlDown(false),
  _selectionMade(false),
  _dropWindow(0),
  _selection(-1)
{
	RECT screenRect;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop,&screenRect);
	screenRect.left = ((screenRect.right / 2) - 200);
	screenRect.top = (screenRect.bottom / 5);
	screenRect.right = (screenRect.left + 400);
	screenRect.bottom = (screenRect.top + 150);
	initialize("Offset Action Input", "Offset Action Input", _globHINST, WS_EX_TOPMOST, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), screenRect, NULL);

	screenRect.left = 70;
	screenRect.top = 65;
	screenRect.right = (screenRect.left + 250);
	screenRect.bottom = (screenRect.top + 320);
	int index(0);
	std::wstring *cueBanner = NULL;
	switch(prevAction) {
		case L_CLICK:													break;
		case L_PRESS:		index = 1;									break;
		case R_CLICK:		index = 2;									break;
		case D_CLICK:		index = 3;									break;
		case SCR_UP:		index = 4;									break;
		case SCR_DOWN:		index = 5;									break;
		case CLICK_N_DRAG:	index = 6;									break;
		case NO_ACT:		index = 7;									break;
		default:			cueBanner = new std::wstring(L"Actions");
	}
	std::vector<std::string> members(8);
	members.at(0) = "Left Click";
	members.at(1) = "Left Press";
	members.at(2) = "Right Click";
	members.at(3) = "Double Click";
	members.at(4) = "Scroll Up";
	members.at(5) = "Scroll Down";
	members.at(6) = "Click & Drag";
	members.at(7) = "No Action";
	_dropWindow.initialize(this, getHWND(), screenRect, cueBanner, index, members);
	delete(cueBanner);

	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);
	showWindow(SW_SHOWNORMAL);
	setForeground();
	update();
	_dropWindow.setFocus();
}


int ActionOffsetMenu::waitForInput()
 {
	MSG msg;
	COMBOBOXINFO cbInfo = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(_dropWindow.getHWND(), &cbInfo);
	while(isEnabled() && GetMessage(&msg, nullptr, 0, 0)) {
		if(msg.hwnd == getHWND() || msg.hwnd == cbInfo.hwndCombo || msg.hwnd == cbInfo.hwndItem || msg.hwnd == cbInfo.hwndList) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return indexToAction(_selection);
}


LRESULT CALLBACK ActionOffsetMenu::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_LBUTTONDOWN:
			_dropWindow.setFocus();
			{
				int x(GET_X_LPARAM(lParam));
				int y(GET_Y_LPARAM(lParam));
				if((y > 61) && (y < 93)) {
					RECT rcClient;
					getClientDim(&rcClient);
					if((x > 10) && (x < 30)) {
						_goBack = true;
						sendMessage(WM_CLOSE, 0, 0);
					}
					else if((x > (rcClient.right - 30)) && (x < (rcClient.right - 10))) {
						_selection = _dropWindow.sendMessage(CB_GETCURSEL, 0, 0);
						if(_selection != CB_ERR) {
							_selectionMade = true;
							postMessage(WM_CLOSE, 0, 0);
						}
					}
				}
			}
			break;
		case WM_PAINT:
			{
				using namespace Gdiplus;
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				RECT rcClient, rcClip;
				GetClientRect(hwnd, &rcClient);
				HDC hdcBuffer = CreateCompatibleDC(hdc);
				HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
				SelectObject(hdcBuffer, hBitmap);
				Graphics graphics(hdcBuffer);
				LinearGradientBrush gradBrush(Point(rcClient.left, rcClient.top), Point(rcClient.right, rcClient.bottom), Color(255, 255, 255), Color(100, 100, 255));
				graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
				SolidBrush sb(Color(0, 0, 0));
				graphics.DrawString(L"What action would you like to perform", 37,  &Font(L"Times New Roman", 17), PointF(7, 7), NULL, &sb);
				graphics.DrawString(L"at the chosen location?", 23,  &Font(L"Times New Roman", 17), PointF(82, 30), NULL, &sb);
				graphics.DrawImage(&Image(L"BackSymbolPop.png"), 10, 62);
				graphics.DrawImage(&Image(L"ForwardSymbolPop.png"), (rcClient.right - 30), 62);
				GetClipBox(hdc, &rcClip);
				BitBlt(hdc, rcClip.left, -rcClip.top, (rcClip.right - rcClip.left), (rcClip.bottom + rcClip.top), hdcBuffer, rcClip.left, -rcClip.top, SRCCOPY);
				DeleteObject(hBitmap);
				DeleteObject(hdcBuffer);
				EndPaint(hwnd, &ps);
			}
			break;
		case WM_KEYDOWN:
			if((wParam == VK_CONTROL) || (wParam == VK_BACK)) {
				if(wParam == VK_CONTROL) {
					_ctrlDown = true;
					setFocus();
				}
				_goBack = (_ctrlDown && wParam == VK_BACK);
				if(_goBack) {
					sendMessage(WM_CLOSE, 0, 0);
				}
				break;
			}
			switch(wParam) {
				case VK_OEM_MINUS:
					showWindow(SW_MINIMIZE);
					break;
				case VK_ESCAPE:
					sendMessage(WM_CLOSE, 0, 0);
					break;
				default:
					_dropWindow.sendMessage(message, wParam, lParam);
					_dropWindow.setFocus();
			}
			break;
		case WM_KEYUP:
			if(wParam == VK_CONTROL) {
				_ctrlDown = false;
			}
			break;
		case WM_CHAR:
			_dropWindow.postMessage(message, wParam, lParam);
			_dropWindow.setFocus();
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_CLOSE:
			UnregisterHotKey(hwnd, 057);
			break;
		case WM_HOTKEY:
			showWindow(SW_RESTORE);
			_dropWindow.setFocus();
			break;
		case WM_CUSTDROPSELMADE:
			int action(HIWORD(wParam));
			if(action == DROP_SEL_BY_L_PRESS || action == DROP_SEL_BY_RETURN_KEY) {
				_selection = (int)lParam;
				_selectionMade = true;
				sendMessage(WM_CLOSE, 0, 0);
			}
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


bool ActionOffsetMenu::wasInputEntered() const
{
	return _selectionMade;
}


bool ActionOffsetMenu::wasBackEntered() const
{
	return _goBack;
}