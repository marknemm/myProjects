#include <sstream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
#include <boost/archive/text_iarchive.hpp>
#include "Main.h"
#include "MainMenuFrame.h"
#include "RecordsContainer.h"
#include "RecordEnforcer.h"
#include "RecordHandler.h"


RecordHandler *_recordHandler;
HINSTANCE _globHINST;
RecordsContainer _recordsContainer;
MainMenuFrame _menuFrame;
bool _createNewSaveFile;
bool _noExit;


/**
-----Description-----
Loads the data for the current instance of the DynaHelper.
-----Inputs-----
RecordsContainer &recordsContainer: A reference to a RecordsContainer variable that will be written to.
*/
void load(RecordsContainer &recordsContainer)
{
	static int reloadCnt(0);
	std::ifstream ifs("DynaHelper.dat");
	if(ifs.is_open()) {
		try {
			boost::archive::text_iarchive ia(ifs);
			ia >> recordsContainer;
			ifs.close();
		}
		catch(boost::archive::archive_exception const &ex) {
			ifs.close();
			std::string debStr("The save file is likely corrupted ----- ");
			debStr += ex.what();
			debStr += "\nReplacing the current save file with a backup of the last save before the error happened.\n";
			SetFileAttributes("DynaHelperBackup.dat", FILE_ATTRIBUTE_NORMAL);
			SetFileAttributes("DynaHelper.dat", FILE_ATTRIBUTE_NORMAL);
			CopyFile("DynaHelperBackup.dat", "DynaHelper.dat", FALSE);
			SetFileAttributes("DynaHelperBackup.dat", FILE_ATTRIBUTE_READONLY);
			SetFileAttributes("DynaHelper.dat", FILE_ATTRIBUTE_READONLY);
			if(reloadCnt++ < 2) {
				_noExit = true;
				debugBreak(DEBUG_LOC, debStr.c_str());
				load(recordsContainer);
			}
			else {  debugBreak(DEBUG_LOC, debStr.c_str());  }
		}
	}
	else if(_createNewSaveFile) {
		std::string debStr("The save file was either not created properly, was moved, or deleted. if you have moved/deleted DynaHelper.dat, ");
		debStr += "please move/restore it to its original location in the release folder where DynaHelper.exe is located.";
		debugBreak(DEBUG_LOC, debStr.c_str());
	}
	else {  _createNewSaveFile = true;  }
	reloadCnt = 0;
}


/**
-----Description-----
Refreshes the main menu screen after an item has been added.
*/
void refresh()
{
	delete(_recordHandler);
	_recordHandler = NULL;
	load(_recordsContainer);
	_recordHandler = new RecordHandler(_recordsContainer);
	_menuFrame.refresh(_recordHandler);
}


void translateToRECT(Gdiplus::Rect &gdiplusRectF, RECT &rect, int deltaY)
{
	rect.left = int(gdiplusRectF.X);
	rect.top = int(gdiplusRectF.Y - deltaY);
	rect.right = int(gdiplusRectF.X + gdiplusRectF.Width);
	rect.bottom = int(gdiplusRectF.Y + gdiplusRectF.Height - deltaY);
}


/**
-----Description-----
Gets the machine's screen dimensions that the DynaHelper is running on.
-----Outputs-----
Rect: Screen dimensions with X & Y at coordinate (0,0).
*/
void getScreenDimensions(RECT &dimensions)
{
	GetWindowRect(GetDesktopWindow(), &dimensions);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG         msg;
	ULONG_PTR	gdiplusToken;
	
	_globHINST = hInstance;
	_recordHandler = NULL;
	_noExit = false;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	RECT dimensions;
	getScreenDimensions(dimensions);
	RECT mainFrameDim = { (dimensions.right - 300), 0, dimensions.right, 500 };
	_menuFrame.initialize(mainFrameDim);

	refresh();
	if(_createNewSaveFile) {
		_recordHandler->save();
	}

	_recordHandler->fix();

	_menuFrame.setForeground();
	_menuFrame.showWindow(iCmdShow);
	_menuFrame.update();
	_menuFrame.setTextFocus();

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	delete(_recordHandler);
    return msg.wParam;
}


/**
-----Description-----
Prints out a message to a message box pop-up based off of the input arguments.
-----Inputs-----
HWND hwnd: The window that the pop-up will be based from.
int i: A number that will be printed out a line above the argument string s.
string s: A string that will be printed out a line below the previous argument int i.
*/
void prnt(HWND hwnd, int i, std::string s)
{
	std::stringstream ss;
	ss << i << "\n" << s;
	MessageBox(hwnd, ss.str().c_str(), ss.str().c_str(), MB_ICONINFORMATION);
}


int indexToAction(int index)
{
	int act(NO_ACT);
	switch(index) {
		case 0:		act = L_CLICK;			break;
		case 1:		act = L_PRESS;			break;
		case 2:		act = R_CLICK;			break;
		case 3:		act = D_CLICK;			break;
		case 4:		act = SCR_UP;			break;
		case 5:		act = SCR_DOWN;			break;
		case 6:		act = CLICK_N_DRAG;		break;
		case 7:		act = TEXT_INP;			break;
		case 8:		act = K_TYPE;			break;
		case 9:		act = K_PRESS;			break;
		case 10:	act = REL_MOUSE_ACT;	break;
	}
	return act;
}


bool isActionValid(int action)
{
	switch(action) {
		case L_CLICK:		return true;
		case L_PRESS:		return true;
		case R_CLICK:		return true;
		case D_CLICK:		return true;
		case SCR_UP:		return true;
		case SCR_DOWN:		return true;
		case CLICK_N_DRAG:	return true;
		case TEXT_INP:		return true;
		case K_TYPE:		return true;
		case K_PRESS:		return true;
		case REL_MOUSE_ACT:	return true;
		case NO_ACT:		return true;
		case DEFAULT_ACT:	return true;
		default:			return false;
	}
}


void debugBreak(const char *debugLoc, const char *errorMsg)
{
	std::stringstream ss;
	ss << std::endl << debugLoc << std::endl << "Error Message: " << errorMsg << std::endl;
	#ifdef _DEBUG
		ss << std::endl;
		OutputDebugString(ss.str().c_str());
		DebugBreak();
	#else
		const time_t rawtime = std::time(0);
		struct tm timeinfo;
		localtime_s(&timeinfo, &rawtime);
		std::vector<char> timeBuffer(200);
		strftime(timeBuffer.data(), timeBuffer.size(), "------ %m/%d/%Y  -  %I:%M %p -----", &timeinfo);
		std::ofstream os;
		os.open("DynaErrorLog.txt", (std::ios::out | std::ios::app));
		os << timeBuffer.data() << std::endl << ss.str().c_str();
		ss << std::endl << "Adding information to error log" << std::endl;
		MessageBox(GetFocus(), ss.str().c_str(), "ERROR MESSAGE", MB_ICONERROR);
		
		void *stack[100];
		unsigned short frames;
		SYMBOL_INFO *symbol;
		
		SymInitialize(GetCurrentProcess(), NULL, TRUE);
		frames = CaptureStackBackTrace(0, 100, stack, NULL);
		symbol = (SYMBOL_INFO*)calloc((sizeof(SYMBOL_INFO) + 256 * sizeof(char)), 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		os << "================== Stack Trace ===================\n";
		for(size_t i(0) ; i < frames ; i++) {
			SymFromAddr(GetCurrentProcess(), (DWORD64)stack[i], 0, symbol);
			ss = std::stringstream();
			ss << (frames - i - 1) << ": " << symbol->Name << std::endl;
			os << ss.str().c_str();
		}
		if(_recordHandler != NULL) {
			std::string recordsInfo("");
			_recordHandler->getRecordsInfoString(recordsInfo);
			os << std::endl << recordsInfo;
		}
		os << std::endl << std::endl << std::endl << std::endl << std::endl;
		
		delete(symbol);
		if(!_noExit) {
			exit(1);
		}
		else {  _noExit = false;  }
	#endif
}


HBRUSH CreateGradientBrush(COLORREF &top, COLORREF &bottom, LPNMCUSTOMDRAW &item)
{
	HBRUSH brush = NULL;
	HDC hdcmem = CreateCompatibleDC(item->hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(item->hdc, item->rc.right-item->rc.left, item->rc.bottom-item->rc.top);
	SelectObject(hdcmem, hbitmap);

	int r1(GetRValue(top));
	int r2(GetRValue(bottom));
	int g1(GetGValue(top));
	int g2(GetGValue(bottom));
	int b1(GetBValue(top));
	int b2(GetBValue(bottom));
	for(int i(0) ; i < item->rc.bottom-item->rc.top ; i++) { 
		int r(r1 + double(i * (r2 - r1) / item->rc.bottom - item->rc.top));
		int g(g1 + double(i * (g2 - g1) / item->rc.bottom - item->rc.top));
		int b(b1 + double(i * (b2 - b1) / item->rc.bottom - item->rc.top));
		brush = CreateSolidBrush(RGB(r, g, b));
		RECT temp = { 0, i, (item->rc.right - item->rc.left), (i + 1) };
		FillRect(hdcmem, &temp, brush);
		DeleteObject(brush);
	}
	HBRUSH pattern = CreatePatternBrush(hbitmap);

	DeleteDC(hdcmem);
	DeleteObject(brush);
	DeleteObject(hbitmap);

	return pattern;
}