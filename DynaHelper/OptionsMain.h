#ifndef  OPTIONS_MAIN_H
#define  OPTIONS_MAIN_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include "MainGlobals.h"
#include "MorphWind.h"
#include "RecordHandler.h"
#include "DataOptionsh.h"


class OptionsMain : public MorphWind {


	DataOptions _dataOpts;


public:
	
	
	OptionsMain();


private:


	void run();
	void paint();
	void mouseMove(LPARAM lParam);
	void leftButtonDown(LPARAM lParam);
	bool close();
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif