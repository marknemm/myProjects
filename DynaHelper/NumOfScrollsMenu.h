#ifndef  NUM_OF_SCROLLS_MENU_H
#define  NUM_OF_SCROLLS_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "MainGlobals.h"
#include "MorphWind.h"


class NumOfScrollsMenu : public MorphWind {

	class ChildNumOfScrollsMenu : public MorphWind {

		friend NumOfScrollsMenu;

		WNDPROC _origEditProc;
		int _numScrolls;
		bool _ctrlDown;
		bool _entered;

		bool isInputValid();
		LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:

		ChildNumOfScrollsMenu();
	};

	ChildNumOfScrollsMenu _cwnd;
	bool _goBack;
	bool _isWindowAlive;


public:


	NumOfScrollsMenu(int prevNumScrolls);
	int waitForInput();
	bool wasInputEntered() const;
	bool wasBackEntered() const;


private:


	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif