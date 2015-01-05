#include "MainMenuText.h"
#include "MainMenuPane.h"


MainMenuText::MainMenuText(MainMenuFrame *menuFrame)
{
	_menuFrame = menuFrame;
}


void MainMenuText::initialize(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent)
{
	MorphWind::initialize(className, hInstance, exStyle, style, dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	sendMessage(WM_SETTEXT, 0, LPARAM("0"));
}


void MainMenuText::setupCustProc()
{
	SetWindowLongPtr(getHWND(), GWLP_USERDATA, (LONG)this);
	_origEditProc = WNDPROC(SetWindowLongPtr(getHWND(), GWLP_WNDPROC, LONG(mainWndProc)));
}


bool MainMenuText::keyDown(UINT message, WPARAM wParam, LPARAM lParam)
{
	int highlightIndex = _menuFrame->getMenuPane()->getHighlightIndex();
	if((wParam == VK_DOWN) && (highlightIndex < (_recordHandler->numOfRecords() - 1))) {
		_menuFrame->getMenuPane()->sendMessage(message, wParam, lParam);
	}
	else if((wParam == VK_UP) && (highlightIndex > 0)) {
		_menuFrame->getMenuPane()->sendMessage(message, wParam, lParam);
	}
	else {  return false;  }
	return true;
}


bool MainMenuText::charIn(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(wParam == 9) {	//TAB
		_menuFrame->getMenuPane()->setFocus();
	}
	else if(wParam == 13) {	//Enter
		enterChoice();
	}
	else if(wParam == 27) {	//ESC
		_menuFrame->sendMessage(WM_CLOSE, 0, 0);
	}
	else if(((wParam >= 'a') && (wParam <= 'z')) || ((wParam >= 'A') && (wParam <= 'Z')) || (wParam == ' ') || (wParam == '_') || (wParam == '-')) {
		_menuFrame->getMenuPane()->setFocus();
		_menuFrame->getMenuPane()->sendMessage(message, wParam, lParam);
	}
	else if((wParam >= '0') && (wParam <= '9')) {
		CallWindowProc(_origEditProc, getHWND(), message, wParam, lParam);
		char text[6] = "";
		sendMessage(WM_GETTEXT, 6, (LPARAM)text);
		int textI = std::atoi(text);
		if(textI < _recordHandler->numOfRecords()) {
			_menuFrame->getMenuPane()->setHighlightIndex(textI);
			_menuFrame->getMenuPane()->scrollToHighlight();
		}
	}
	else {  return false;  }
	return true;
}


void MainMenuText::mouseWheel(WPARAM wParam, LPARAM lParam)
{
	_menuFrame->getMenuPane()->sendMessage(WM_MOUSEWHEEL, wParam, lParam);
}


LRESULT CALLBACK MainMenuText::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_KEYDOWN:
			if(keyDown(message, wParam, lParam)) {
				return 0;
			}
			break;
		case WM_SETFOCUS:
			sendMessage(EM_SETSEL, 0, MAKELONG(0, 5));
			break;
		case WM_CHAR:
			if(charIn(message, wParam, lParam)) {
				return 0;
			}
			break;
		case WM_MOUSEWHEEL:
			mouseWheel(wParam, lParam);
			return 0;
	}
	return CallWindowProc(_origEditProc, hwnd, message, wParam, lParam);
}


void MainMenuText::enterChoice() {
	char choiceS[7] = "";
	SendMessage(getHWND(), WM_GETTEXT, 7, (LPARAM)choiceS);
	int choice = std::atoi(choiceS);
	if(choice < _recordHandler->numOfRecords() && !_recordHandler->isRecordCleared(choice)) {
		_menuFrame->showWindow(SW_MINIMIZE);
		Sleep(400);
		if(choice == 0) {
			_menuFrame->setActive(false);
			_menuFrame->unregisterHotkey();
			_menuFrame->showWindow(SW_HIDE);
			Sleep(250);
			RecordWithScreen recordWithScreen;
		}
		else {
			RecordEnforcer(*(_recordHandler->getRecord(choice)));
		}
		_menuFrame->getMenuPane()->setHighlightIndex(0);
		sendMessage(WM_SETTEXT, 0, (LPARAM)TEXT("0"));
		sendMessage(EM_SETSEL, 0, MAKELONG(0, 1));
	}
}


void MainMenuText::updateText() {
	std::stringstream ss;
	ss << _menuFrame->getMenuPane()->getHighlightIndex();
	sendMessage(WM_SETTEXT, 0, (LPARAM)ss.str().c_str());
	sendMessage(EM_SETSEL, 0, MAKELONG(0, 5));
}


void MainMenuText::refresh(RecordHandler *recordHandler)
{
	_recordHandler = recordHandler;
	_menuFrame->setText0();
}