#ifndef  MAIN_MENU_FRAME_H
#define  MAIN_MENU_FRAME_H
#include <Windows.h>
#include <gdiplus.h>
#include "RecordHandler.h"
#include "MorphWind.h"
#include "Main.h"
#include "OptionsMain.h"


class MainMenuText; //Some cool default class declarations to declare pointers and actually define them in the compilation unit (.cpp file)
class MainMenuPane;


class MainMenuFrame : public MorphWind {

	bool _active;
	bool _augmentHelp;
	bool _highlightOpt;
	RECT _backgroundRect;
	Gdiplus::RectF _optionsRect;
	Gdiplus::PathGradientBrush *_optionsPGB;
	MainMenuText *_textField;
	MainMenuPane *_menuPane;

public:


	MainMenuFrame();
	~MainMenuFrame();
	void initialize(RECT &dim);
	void setActive(bool active);
	void close();
	void refresh(RecordHandler *recordHandler);
	void registerHotkey();
	void unregisterHotkey();
	void setTextFocus();
	void setText0();
	void showPane();
	void showText();
	void updatePane();
	void updateText();
	MainMenuText* getMenuText();
	MainMenuPane* getMenuPane();


private:


	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void create();
	void mouseMove(LPARAM lParam);
	void leftButtonUp(LPARAM lParam);
	void hotKey();
	void paint();
	void setFocusEvent();
};


#endif