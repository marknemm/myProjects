#ifndef  MAIN_H
#define  MAIN_H

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define  STRINGIZE(x) #x
#define  STR(x) STRINGIZE(x)
#define  DEBUG_LOC "File: " __FILE__ "\nFunction: " __FUNCTION__ "\nLine: " STR(__LINE__)
#include <string>
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <DbgHelp.h>
#include <gdiplus.h>
#include <sstream>
#include <fstream>
#include <ctime>
#include "MainGlobals.h"


void refresh();
void getScreenDimensions(RECT &dimensions);
void translateToRECT(Gdiplus::Rect &gdiplusRectF, RECT &rect, int deltaY);
void prnt(HWND hwnd, int i, std::string s);
void debugBreak(const char *debugLoc, const char *errorMsg);
HBRUSH CreateGradientBrush(COLORREF &top, COLORREF &bottom, LPNMCUSTOMDRAW &item);


#endif