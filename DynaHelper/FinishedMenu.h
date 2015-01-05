#ifndef  FINISHED_MENU_H
#define  FINISHED_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include "MainGlobals.h"
#include "MorphWind.h"


class FinishedMenu : public MorphWind {

	bool _decision;
	bool _entered;
	bool _highlightYes;
	bool _highlightNo;
	bool _goBack;
	bool _ctrlDown;
	bool _isWindowActive;
	Gdiplus::RectF yesRect;
	Gdiplus::RectF noRect;
	Gdiplus::PathGradientBrush *yesPgb;
	Gdiplus::PathGradientBrush *noPgb;
	Gdiplus::SolidBrush *sbBlack;
	Gdiplus::SolidBrush *sbDGray;
	Gdiplus::SolidBrush *sbHlght;

public:


	FinishedMenu();
	bool waitForDecision();
	bool wasInputEntered();
	bool wasBackEntered() const;


private:


	void initYesRect();
	void initNoRect();
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif