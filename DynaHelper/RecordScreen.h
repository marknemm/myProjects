#ifndef  RECORD_SCREEN_H
#define  RECORD_SCREEN_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include <boost/thread/thread.hpp>
#include "MainGlobals.h"
#include "ActionMacros.h"
#include "Point.h"
#include "MorphWind.h"


class RecordScreen : public MorphWind {

	bool _run;
	bool _setToWork;
	bool _pressed;
	bool _didMouseMove;
	bool _returnable;
	bool _returnOn;
	bool _goBack;
	bool _drawTrajectoryLine;
	bool _paintBackSymbol;
	int _action;
	int _startMS;
	UINT _prevStyle;
	int _keyPressed;
	Point _location;
	Point _location2;
	POINT _pt1;
	POINT _pt2;
	Gdiplus::RectF _backRect;
	Gdiplus::RectF _resolution;
	Gdiplus::RectF _textBoundBox;
	std::wstring _displayText;

public:


	RecordScreen(bool paintBackSymbol);
	//void initialize(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, MorphWind *parent);
	void waitForInput(const int action);
	bool wasBackEntered() const;
	int keyEntered() const;
	Point locationEntered();
	Point location2Entered();
	void updateNow();
	void setLayeredWindowAttributes(COLORREF colorRef, BYTE bAlpha, DWORD dwFlags);


private:


	void setScreenEnabled(const int action);
	void returnTimer();
	void mouseMove(LPARAM lParam);
	void leftButtonDown(LPARAM lParam);
	void leftButtonUp(LPARAM lParam);
	void keyDown(WPARAM wParam);
	void drawTrajectoryLine(Gdiplus::Graphics &graphics, RECT &rcClip);
	void paint();
	void hotKey();
	void getMinMaxInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void close();
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif