#ifndef  MAIN_MENU_TEXT_H
#define  MAIN_MENU_TEXT_H
#include "MorphWind.h" 
#include "RecordHandler.h"
#include "MainMenuFrame.h"
#include "RecordEnforcer.h"
#include "RecordWithScreen.h"
#include <sstream>


class MainMenuText : public MorphWind {

	friend MainMenuFrame;

private:


	WNDPROC _origEditProc;
	MainMenuFrame *_menuFrame;


public:


	MainMenuText(MainMenuFrame *menuFrame);
	void initialize(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent);
	void setupCustProc();
	void updateText();
	void enterChoice();
	void refresh(RecordHandler *recordHandler);


private:


	bool keyDown(UINT message, WPARAM wParam, LPARAM lParam);
	bool charIn(UINT message, WPARAM wParam, LPARAM lParam);
	void mouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif