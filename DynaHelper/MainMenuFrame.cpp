#include "MainMenuFrame.h"
#include "MainMenuText.h"
#include "MainMenuPane.h"


MainMenuFrame::MainMenuFrame()
: _optionsRect(Gdiplus::REAL(3), Gdiplus::REAL(3), Gdiplus::REAL(50), Gdiplus::REAL(13)),
  _active(true),
  _augmentHelp(false),
  _highlightOpt(false)
{}


MainMenuFrame::~MainMenuFrame()
{
	delete(_textField, _menuPane);
}


void MainMenuFrame::initialize(RECT &dim)
{
	MorphWind::initialize("DynaHelper - Main Menu", "DynaHelper - Main Menu", _globHINST, WS_EX_APPWINDOW
      , (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), dim, NULL);
	RECT textDim = { 177, 25, 257, 55 };
	RECT paneDim = { 5, 84, 280, 459 };
	_textField = new MainMenuText(this);
	_menuPane = new MainMenuPane(this);
	_textField->initialize("EDIT", _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER), textDim, this);
	_textField->setupCustProc();
	_menuPane->initialize("DynaHelper - Main Menu Pane", NULL, _globHINST, NULL, (WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL), paneDim, this);

	Gdiplus::GraphicsPath gPath;
	gPath.AddRectangle(_optionsRect);
	int numClrs(1);
	_optionsPGB = new Gdiplus::PathGradientBrush(&gPath);
	Gdiplus::Color surClr(175, 175, 255);
	_optionsPGB->SetCenterColor(Gdiplus::Color(255, 255, 255));
	_optionsPGB->SetSurroundColors(&surClr, &numClrs);
}


void MainMenuFrame::create()
{
	SetRect(&_backgroundRect, 0, 0, 300, 500);
}


void MainMenuFrame::hotKey()
{
	if(_active) {
		_menuPane->_search = "";
		_menuPane->resetHighlight();
		_textField->updateText();
		_menuPane->update();
		showWindow(SW_RESTORE);
		setForeground();
		_textField->setFocus();
		_menuPane->scrollToHighlight();
	}
}


void MainMenuFrame::mouseMove(LPARAM lParam)
{
	int x, y;
	bool inOpt, inHelp;
	RECT clientRect;
	x = GET_X_LPARAM(lParam);
	y = GET_Y_LPARAM(lParam);
	getClientDim(&clientRect);
	RECT redrawRect = { 1, 1, clientRect.right, 20 };
	inOpt = (x >= (int)_optionsRect.GetLeft() && y >= (int)_optionsRect.GetTop() && x <= (int)_optionsRect.GetRight() && y <= (int)_optionsRect.GetBottom());
	inHelp = (x >= (clientRect.right - 16) && y >= 3 && x <= (clientRect.right - 3) && y <= 13);
	if(inOpt && !_highlightOpt) {
		_highlightOpt = true;
		_augmentHelp = false;
		RedrawWindow(getHWND(), &redrawRect, NULL, RDW_INVALIDATE);
	}
	else if(inHelp && !_augmentHelp) {
		_augmentHelp = true;
		_highlightOpt = false;
		RedrawWindow(getHWND(), &redrawRect, NULL, RDW_INVALIDATE);
	}
	else if(!inHelp && !inOpt && (_augmentHelp || _highlightOpt)) {
		_augmentHelp = false;
		_highlightOpt = false;
		RedrawWindow(getHWND(), &redrawRect, NULL, RDW_INVALIDATE);
	}
}


void MainMenuFrame::leftButtonUp(LPARAM lParam)
{
	if(_highlightOpt) {
		showWindow(SW_MINIMIZE);
		showWindow(SW_HIDE);
		_active = false;
		OptionsMain optionsMain;
		::refresh();
	}
	else if(_augmentHelp) {
		// TODO: +++Some help docs will be brought up here int he future+++
	}
	else {  _menuPane->setFocus();  }
}


void MainMenuFrame::paint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(getHWND(), &ps);
	RECT rcClient;
	GetClientRect(getHWND(), &rcClient);
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
	SelectObject(hdcBuffer, hBitmap);
	Gdiplus::Graphics graphics(hdcBuffer);
	Gdiplus::LinearGradientBrush gradBrush(Gdiplus::Point(rcClient.left, rcClient.top), Gdiplus::Point(rcClient.right, rcClient.bottom), Gdiplus::Color(255, 255, 255), Gdiplus::Color(100, 100, 255));
	graphics.FillRectangle(&gradBrush, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
	RECT textRect = {100, 50, 200, 130};
	Gdiplus::SolidBrush sb(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::Font font(L"Arial", 25);
	graphics.DrawString(L"Selection: ", 11, &font, Gdiplus::PointF(Gdiplus::REAL(20), Gdiplus::REAL(20)), NULL, &sb);
	if(_augmentHelp) {
		graphics.DrawImage(&Gdiplus::Image(L"HelpIcon.png", 0), (rcClient.right - 17), 2, 15, 15);
	}
	else {
		graphics.DrawImage(&Gdiplus::Image(L"HelpIcon.png", 0), (rcClient.right - 16), 3, 13, 13);
	}
	graphics.FillRectangle(_optionsPGB, _optionsRect);
	Gdiplus::StringFormat sform;
	sform.SetAlignment(Gdiplus::StringAlignmentCenter);
	if(_highlightOpt) {
		graphics.DrawString(L"Options", 7, &Gdiplus::Font(L"Arial", 7), _optionsRect, &sform, &Gdiplus::SolidBrush(Gdiplus::Color(150, 150, 150)));
	}
	else {
		graphics.DrawString(L"Options", 7, &Gdiplus::Font(L"Arial", 7), _optionsRect, &sform, &sb);
	}
	RECT rcClip;
	GetClipBox(hdc, &rcClip);
	BitBlt(hdc, rcClip.left, -rcClip.top, (rcClip.right - rcClip.left), (rcClip.bottom + rcClip.top), hdcBuffer, rcClip.left, -rcClip.top, SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(hdcBuffer);
	EndPaint(getHWND(), &ps);
}


void MainMenuFrame::setFocusEvent() {
	setForeground();
	_textField->setFocus();
}


LRESULT CALLBACK MainMenuFrame::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
		case WM_HOTKEY:			hotKey();				break;		//ctrl + w
		case WM_CREATE:			create();				break;
		case WM_PAINT:			paint();				break;
		case WM_MOUSEMOVE:		mouseMove(lParam);		break;
		case WM_SETFOCUS:		setFocusEvent();		break;
		case WM_DESTROY:		PostQuitMessage(0);		break;
		case WM_LBUTTONUP:		leftButtonUp(lParam);	return 0;
		case WM_ERASEBKGND:								return 0;	//Don't erase background (stops flickering on repaint)
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


void MainMenuFrame::setActive(bool active)
{
	_active = active;
}


void MainMenuFrame::close()
{
	SendMessage(getHWND(), WM_DESTROY, NULL, NULL);
}


void MainMenuFrame::registerHotkey()
{
	RegisterHotKey(getHWND(), 057, MOD_CONTROL, 0x57);
}


void MainMenuFrame::unregisterHotkey()
{
	UnregisterHotKey(getHWND(), 057);
}


void MainMenuFrame::refresh(RecordHandler *recordHandler)
{
	static bool startup(true);
	_augmentHelp = false;
	_highlightOpt = false;
	_menuPane->refresh(recordHandler);
	_textField->refresh(recordHandler);
	registerHotkey();
	_active = true;
	if(startup) {
		startup = false;
	}
	else {
		showWindow(SW_MINIMIZE);
		sendMessage(WM_HOTKEY, NULL, NULL);
	}
}


MainMenuText* MainMenuFrame::getMenuText()
{
	return _textField;
}


MainMenuPane* MainMenuFrame::getMenuPane()
{
	return _menuPane;
}


void MainMenuFrame::setTextFocus()
{
	_textField->setFocus();
}


void MainMenuFrame::setText0()
{
	_textField->sendMessage(EM_SETSEL, 0, MAKELONG(0, 1));
}


void MainMenuFrame::showPane()
{
	_menuPane->showWindow(SW_SHOWNORMAL);
}


void MainMenuFrame::showText()
{
	_textField->showWindow(SW_SHOWNORMAL);
}


void MainMenuFrame::updatePane()
{
	_menuPane->update();
}


void MainMenuFrame::updateText()
{
	_textField->update();
}