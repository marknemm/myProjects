#ifndef  MORPH_WIND_H
#define  MORPH_WIND_H
#include <Windows.h>
#include <windowsx.h>
#include "Main.h"


/**
 * A base class to be used by any individual window to support oop.  This is how it works:
 *
 * oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
 *
 * The mainWndProc is the static window procedure that is used to support C version for win32 api.
 * We have an encapsulated wndProc that is inhereted by the derived class which is called from inside of
 * mainWndProc.  In order to call the correct instance of wndProc inside of the static mainWndProc, we give
 * each instance of a window a "this" ptr to the instance of the derived class when we call CreateWindow.
 * It gets stored as the (void*)lpParam (last arg. of CreateWindow).  This is then retreived in mainWndProc
 * each time by GetWindowLong with flag GWA_USERDATA after initially being set as the extra data for the window
 * in WM_CREATE.
 */
class MorphWind {

	HWND _hwnd;
	MorphWind *_parent;
	HINSTANCE _hInstance;
	HBITMAP _hBitmap;
	LPCSTR _className;
	bool _isAlive;
	bool _isWindowsControl;

public:


	MorphWind(); //DEFAULT_ACT Constructor for late initialization
	MorphWind(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent);
	MorphWind(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent);
	MorphWind(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent);
	MorphWind(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent);
	MorphWind(HWND hwnd, HINSTANCE hInstance, MorphWind *parent, LPCSTR className);
	~MorphWind();
	//Following two methods used for late initialization in conjunction with DEFAULT_ACT (first) constructor
	virtual void initialize(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent);
	virtual void initialize(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent);
	virtual void initialize(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent);
	virtual void initialize(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent);
	virtual void initialize(HWND hwnd, HINSTANCE hInstance, MorphWind *parent, LPCSTR className);
	void showWindow(int nCmdShow);
	void setDim(HWND hInsertAfter, RECT &dim, UINT flags);
	void getDim(LPRECT rect);
	void getClientDim(LPRECT rect);
	void setFocus();
	void setForeground();
	void invalidate(RECT *rect);
	void update();
	void redrawWindow(const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags); 
	void enable(bool flag);
	LRESULT sendMessage(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT postMessage(UINT message, WPARAM wParam, LPARAM lParam);
	bool peekMessage(LPMSG lpMsg, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
	BOOL isWindowVisible() const;
	BOOL isMinimized() const;
	BOOL isEnabled() const;
	BOOL isHwndEqual(HWND hwnd);
	MorphWind& getParent() const;
	HWND MorphWind::getParentHWND() const;


protected:


	// These can be used to define more function in derived class for changing/accessing members of window
	HWND getHWND() const;
	HINSTANCE getHINSTANCE() const;
	virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


private:


	void registerWind(HINSTANCE hInstance, LPCSTR className);
	void createWind(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent);
};


#endif