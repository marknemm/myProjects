#include "CustDropWindow.h"


DropWindow::DropWindow(unsigned short dropId)
: _cbInfo(),
  _dropList(),
  _dropId(dropId)
{}


DropWindow::DropList::DropList(){}


//DropWindow::DropEdit::DropEdit(){}


void DropWindow::initialize(MorphWind *parent, HWND hParent, RECT &dim, std::wstring *cue, u_int selectIndex, std::vector<std::string> &members)
{
	HWND hwnd = CreateWindowEx(NULL, WC_COMBOBOX, "", (CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_BORDER), dim.left, dim.top, (dim.right - dim.left)
	  , (dim.bottom - dim.top), hParent, NULL, _globHINST, this);
	MorphWind::initialize(hwnd, _globHINST, parent, WC_COMBOBOX);

	for(std::vector<std::string>::iterator it(members.begin()) ; it != members.end() ; it++) {
		sendMessage(CB_ADDSTRING, NULL, LPARAM(it->c_str()));
	}
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(FALSE, 0));
	
	if(cue == NULL) {
		sendMessage(CB_SETCURSEL, selectIndex, 0);
	}
	else {  sendMessage(CB_SETCUEBANNER, 0, LPARAM(cue->c_str()));  }

	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origComboProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(MorphWind::mainWndProc)));

	_cbInfo.cbSize = sizeof(COMBOBOXINFO);
	GetComboBoxInfo(hwnd, &_cbInfo);
	_dropList.wrapDropList(_cbInfo.hwndList, this);
}


void DropWindow::DropList::wrapDropList(HWND hDropList, DropWindow *pDropWindow)
{
	_pDropWindow = pDropWindow;
	MorphWind::initialize(hDropList, _globHINST, pDropWindow, WC_LISTBOX);
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origListProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(MorphWind::mainWndProc)));
}


/*void DropWindow::DropEdit::wrapDropEdit(HWND hDropList, MorphWind *parent)
{
	MorphWind::initialize(hDropList, _globHINST, parent, WC_EDIT);
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, LONG(this));
	_origEditProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(MorphWind::mainWndProc)));
}*/


LRESULT CALLBACK DropWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int selection(CB_ERR);
	switch(message) {
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_CONTROL:	getParent().sendMessage(message, wParam, lParam);	break;
				case VK_ESCAPE:		getParent().sendMessage(message, wParam, lParam);	break;
				case VK_RETURN:
					{
						int selection(sendMessage(CB_GETCURSEL, NULL, NULL));
						if(selection != CB_ERR) {
							getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_dropId), DROP_SEL_BY_RETURN_KEY), selection);
							return 0;
						}
					}
					break;
				default:
					sendMessage(CB_SHOWDROPDOWN, TRUE, 0);
			}
			break;
		case WM_CHAR:
			{
				sendMessage(CB_SHOWDROPDOWN, TRUE, 0);
				CallWindowProc(_origComboProc, hwnd, message, wParam, lParam);
				int selection(sendMessage(CB_GETCURSEL, NULL, NULL));
				if(selection != CB_ERR) {
					getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_dropId), DROP_SEL_BY_CHAR_IN), selection);
					return 0;
				}
			}
			break;
		case CB_SETCURSEL:
			if(selection == wParam) {
				return 0;
			}
			else {  selection = wParam;  }
			break;
	}
	return CallWindowProc(_origComboProc, hwnd, message, wParam, lParam);
}


LRESULT CALLBACK DropWindow::DropList::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_LBUTTONDOWN:
			{
				int x(GET_X_LPARAM(lParam));
				int y(GET_Y_LPARAM(lParam));
				RECT winRect;
				getDim(&winRect);
				if(x >= 0 && x <= (winRect.right - winRect.left) && y >= 0 && y <= (winRect.bottom - winRect.top)) {
					CallWindowProc(_origListProc, hwnd, message, wParam, lParam);
					int selection(getParent().sendMessage(CB_GETCURSEL, NULL, NULL));
					if(selection != CB_ERR) {
						getParent().getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_pDropWindow->_dropId), DROP_SEL_BY_L_PRESS), selection);
						return 0;
					}
				}
			}
			break;
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_RETURN:
					{
						int selection(getParent().sendMessage(CB_GETCURSEL, NULL, NULL));
						if(selection != CB_ERR) {
							getParent().getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_pDropWindow->_dropId), DROP_SEL_BY_RETURN_KEY), selection);
							return 0;
						}
					}
					break;
				case VK_DOWN:
					{
						CallWindowProc(_origListProc, hwnd, message, wParam, lParam);
						int selection(getParent().sendMessage(CB_GETCURSEL, NULL, NULL));
						if(selection != CB_ERR) {
							getParent().getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_pDropWindow->_dropId), DROP_SEL_BY_DOWN_KEY), selection);
						}
					}
					return 0;
				case VK_UP:
					{
						CallWindowProc(_origListProc, hwnd, message, wParam, lParam);
						int selection(getParent().sendMessage(CB_GETCURSEL, NULL, NULL));
						if(selection != CB_ERR) {
							getParent().getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_pDropWindow->_dropId), DROP_SEL_BY_UP_KEY), selection);
						}
					}
					return 0;
			}
			break;
		case WM_MOUSEMOVE:
			{
				int x(GET_X_LPARAM(lParam));
				int y(GET_Y_LPARAM(lParam));
				RECT winRect;
				getDim(&winRect);
				if(x >= 0 && x <= (winRect.right - winRect.left) && y >= 0 && y <= (winRect.bottom - winRect.top)) {
					int newSel(y / getParent().sendMessage(CB_GETITEMHEIGHT, 0, 0));
					getParent().sendMessage(CB_SETCURSEL, newSel, 0);
					getParent().getParent().sendMessage(WM_CUSTDROPSELMADE, MAKEWPARAM(int(_pDropWindow->_dropId), DROP_SEL_BY_MOUSE_MOVE), newSel);
				}
			}
			break;
	}
	return CallWindowProc(_origListProc, hwnd, message, wParam, lParam);
}


/*LRESULT CALLBACK DropWindow::DropEdit::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(wParam == VK_RETURN) {
				int selection(getParent().sendMessage(CB_GETCURSEL, NULL, NULL));
				if(selection != CB_ERR) {
					getParent().getParent().sendMessage(WM_CUSTDROPSELMADE, DROP_SEL_BY_RETURN_KEY, 0);
					return 0;
				}
			}
			break;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}*/


HWND DropWindow::getHWND() const
{
	return MorphWind::getHWND();
}


unsigned short DropWindow::getDropId() const
{
	return _dropId;
}