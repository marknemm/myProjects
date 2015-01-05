#ifndef  ACTION_MENU_H
#define  ACTION_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include "MainGlobals.h"
#include "ActionMacros.h"
#include "MorphWind.h"
#include "CustDropWindow.h"


class ActionMenu : public MorphWind {


	DropWindow _dropWindow;
	bool _goBack;
	bool _ctrlDown;
	bool _ableGoBack;
	bool _selectionMade;
	int _selection;


public:


	ActionMenu();
	ActionMenu(int indexAction, bool ableGoBack);
	int waitForInput();
	bool isActionSelected() const;
	bool wasBackEntered() const;


private:


	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif