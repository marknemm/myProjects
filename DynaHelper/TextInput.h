#ifndef  TEXT_INP_H
#define  TEXT_INP_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <string>
#include "MainGlobals.h"
#include "MorphWind.h"


class TextInput : public MorphWind {

	class ChildTextInput : public MorphWind {

		friend TextInput;

		WNDPROC _origEditProc;
		std::string _input;
		bool _ctrlDown;
		bool _entered;

		bool isInputValid();
		LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:

		ChildTextInput();
	};

	ChildTextInput _cwnd;
	bool _goBack;
	bool _isWindowAlive;


public:
	
	
	TextInput();
	TextInput(const std::string &prevString);
	std::string waitForInput();
	bool wasInputEntered() const;
	bool wasBackEntered() const;


private:

	
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif