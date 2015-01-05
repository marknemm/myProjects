#ifndef  MAIN_MENU_PANE_H
#define  MAIN_MENU_PANE_H
#include "MorphWind.h"
#include "RecordHandler.h"
#include "MainMenuFrame.h"
#include "Main.h"
#include <sstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <Windowsx.h>
#include <gdiplus.h>


class MainMenuPane : public MorphWind {

	friend MainMenuFrame;

private:


	MainMenuFrame *_menuFrame;
	RecordHandler *_recordHandler;
	std::vector<Gdiplus::Rect> _bounds;
	std::vector<int> _indices;
	int _highlightIndex;
	int _prevHighlightIndex;
	int _deltaY;
	int _height;
	POINT _prevMouseLoc;
	std::string _search;


public:


	MainMenuPane(MainMenuFrame *menuFrame);
	void setHighlightIndex(const unsigned int index);
	void resetHighlight();
	void refresh(RecordHandler *recordHandler);
	int getHighlightIndex();
	void setHighlightIndex(int index);
	void scrollToHighlight();


private:

	void create(HWND hwnd);
	void paint();
	void mouseMove(LPARAM lParam);
	void mouseWheel(WPARAM wParam, LPARAM lParam);
	void keyDown(WPARAM wParam);
	void charIn(WPARAM wParam);
	void leftButtonDown(LPARAM lParam);
	void vertScroll(HWND hwnd, WPARAM wParam);
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif