#ifndef  TITLE_MENU_H
#define  TITLE_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <random>
#include <time.h>
#include "MainGlobals.h"
#include "MorphWind.h"


class TitleMenu : public MorphWind {

	class ChildTitleMenu : public MorphWind {

		friend TitleMenu;

		WNDPROC _origEditProc;
		std::string _title;
		bool _entered;
		bool _ctrlDown;

		bool isInputValid();
		LRESULT CALLBACK wndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

	public:

		ChildTitleMenu();
	};

	ChildTitleMenu _cwnd;
	bool _goBack;
	bool _isWindowAlive;

public:


	TitleMenu(int randomize, const std::string &prevTitle);
	std::string waitForTitle();
	bool wasTitleEntered() const;
	bool wasBackEntered() const;


private:


	 LRESULT CALLBACK wndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
};


#endif