#ifndef  ACTION_DELAY_MENU_H
#define  ACTION_DELAY_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "MainGlobals.h"
#include "MorphWind.h"


class ActionDelayMenu;


class ChildActionDelayMenu : public MorphWind {

	friend ActionDelayMenu;

	double _delayms;
	bool _ctrlDown;
	bool _entered;
	WNDPROC _origEditProc;
	
public:


	ChildActionDelayMenu();


private:


	bool isInputValid();
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


class ActionDelayMenu : public MorphWind {

	ChildActionDelayMenu _cwnd;
	static bool _wasInitialized;
	bool _isWindowOpen;
	bool _goBack;


public:


	ActionDelayMenu();
	ActionDelayMenu(bool lowerDelayOk, double prevDelay);
	int waitForDelay(bool lowerDelayOk);
	bool wasDelayEntered() const;
	bool wasBackEntered() const;


private:


	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif