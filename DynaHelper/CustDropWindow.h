#ifndef  CUST_DROP_WINDOW_H
#define  CUST_DROP_WINDOW_H
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <vector>
#include <string>
#include "MorphWind.h"


#define WM_CUSTDROPSELMADE	(WM_APP + 21)
#define DROP_SEL_BY_L_PRESS		0
#define DROP_SEL_BY_RETURN_KEY	1
#define DROP_SEL_BY_DOWN_KEY	2
#define DROP_SEL_BY_UP_KEY		3
#define DROP_SEL_BY_CHAR_IN		4
#define DROP_SEL_BY_MOUSE_MOVE	5


class DropWindow : public MorphWind {
	
	
	class DropList : public MorphWind {
			WNDPROC _origListProc;
			DropWindow *_pDropWindow;
		public:
			DropList();
			void wrapDropList(HWND hDropList, DropWindow *pDropWindow);
		private:
			LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	};


	friend DropList;
	u_short _dropId;
	COMBOBOXINFO _cbInfo;
	DropList _dropList;
	WNDPROC _origComboProc;


public:

	
	DropWindow(u_short dropId);
	void initialize(MorphWind *parent, HWND hParent, RECT &dim, std::wstring *cue, u_int selectInd, std::vector<std::string> &members);
	HWND getHWND() const;
	u_short getDropId() const;
	

private:

	
	LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif