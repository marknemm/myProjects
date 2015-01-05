#ifndef  PRESS_DURATION_MENU_H
#define  PRESS_DURATION_MENU_H
#include <Windows.h>
#include <Windowsx.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "MainGlobals.h"
#include "MorphWind.h"


class PressDurationMenu : public MorphWind {

	class ChildPressDurationMenu : public MorphWind {

		friend PressDurationMenu;

		WNDPROC _origEditProc;
		double _durations;
		bool _ctrlDown;
		bool _entered;

		bool isInputValid();
		LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:

		ChildPressDurationMenu();
	};

	ChildPressDurationMenu _cwnd;
	bool _goBack;
	bool _isWindowAlive;


public:


	PressDurationMenu(double prevDuration);
	int waitForDuration();
	bool wasDurationEntered() const;
	bool wasBackEntered() const;


private:


	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif