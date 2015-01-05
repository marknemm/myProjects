#ifndef  ACTION_OFFSET_MENU_H
#define  ACTION_OFFSET_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include "MainGlobals.h"
#include "ActionMacros.h"
#include "MorphWind.h"
#include "CustDropWindow.h"


class ActionOffsetMenu : public MorphWind {


	DropWindow _dropWindow;
	bool _goBack;
	bool _ctrlDown;
	bool _selectionMade;
	int _selection;


public:


	ActionOffsetMenu(int prevAction);
	bool wasInputEntered() const;
	bool wasBackEntered() const;
	int waitForInput();


private:


	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif