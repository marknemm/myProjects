#ifndef  KEY_INPUT_VERIFICATION_MENU_H
#define  KEY_INPUT_VERIFICATION_MENU_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include "MainGlobals.h"
#include "MorphWind.h"


class KeyInputVerificationMenu : public MorphWind {

	bool _decision;
	bool _entered;
	bool _goBack;
	bool _ctrlDown;
	bool _isWindowAlive;
	bool _highlightYes;
	bool _highlightNo;
	Gdiplus::RectF clientRect;
	Gdiplus::RectF yesRect;
	Gdiplus::RectF noRect;
	Gdiplus::PathGradientBrush *yesPgb;
	Gdiplus::PathGradientBrush *noPgb;
	Gdiplus::SolidBrush *sbBlack;
	Gdiplus::SolidBrush *sbDGray;
	Gdiplus::SolidBrush *sbHlght;
	std::wstring displayText;

public:


	KeyInputVerificationMenu(int keyCode);
	bool waitForDecision();
	bool wasInputEntered() const;
	bool wasBackEntered() const;


private:

	
	void initYesRect();
	void initNoRect();
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif