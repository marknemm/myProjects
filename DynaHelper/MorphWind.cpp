#include "MorphWind.h"


MorphWind::MorphWind(){}


MorphWind::MorphWind(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent)
{
	initialize(className, name, hInstance, exStyle, style, dim, hmenu, parent);
}


MorphWind::MorphWind(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent)
: _isWindowsControl(true)
{
	initialize(className, hInstance, exStyle, style, dim, hmenu, parent);
}


MorphWind::MorphWind(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent)
{
    initialize(className, name, hInstance, exStyle, style, dim, parent);
}


/**
 * Constructor for child window control (such as "EDIT")
 */
MorphWind::MorphWind(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent)
{
	initialize(className, NULL, hInstance, exStyle, style, dim, parent);
}


MorphWind::MorphWind(HWND hwnd, HINSTANCE hInstance, MorphWind *parent, LPCSTR className)
{
	_className = className;
	_hInstance = hInstance;
	_parent = parent;
	_hwnd = hwnd;
}


MorphWind::~MorphWind(){}


void MorphWind::initialize(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent)
{
	_isWindowsControl = false;
	_className = className;
	_hInstance = hInstance;
	_parent = parent;
	registerWind(hInstance, className);
	createWind(className, name, hInstance, exStyle, style, dim, hmenu, parent);
}


void MorphWind::initialize(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent)
{
	_isWindowsControl = true;
	_className = className;
	_hInstance = hInstance;
	_parent = parent;
	createWind(className, NULL, hInstance, exStyle, style, dim, NULL, parent);
}


void MorphWind::initialize(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent)
{
	_isWindowsControl = false;
	_className = className;
	_hInstance = hInstance;
	_parent = parent;
	registerWind(hInstance, className);
	createWind(className, name, hInstance, exStyle, style, dim, NULL, parent);
}


void MorphWind::initialize(LPCSTR className, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, MorphWind *parent)
{
	_isWindowsControl = true;
	_className = className;
	_hInstance = hInstance;
	_parent = parent;
	createWind(className, NULL, hInstance, exStyle, style, dim, NULL, parent);
}


void MorphWind::initialize(HWND hwnd, HINSTANCE hInstance, MorphWind *parent, LPCSTR className)
{
	_className = className;
	_hInstance = hInstance;
	_parent = parent;
	_hwnd = hwnd;
}


void MorphWind::registerWind(HINSTANCE hInstance, LPCSTR className)
{
	WNDCLASSEX wndclass;
	wndclass.cbSize		   = sizeof(wndclass);
	wndclass.style         = (CS_HREDRAW | CS_VREDRAW);
	wndclass.lpfnWndProc   = mainWndProc;
	wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = HBRUSH(GetStockObject(WHITE_BRUSH));
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = className;
	wndclass.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);
	if(!RegisterClassEx(&wndclass) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)) {
		std::stringstream ss;
		ss << "Window class registration failed with error code: " << GetLastError();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
}


void MorphWind::createWind(LPCSTR className, LPCSTR name, HINSTANCE hInstance, UINT exStyle, UINT style, RECT &dim, HMENU hmenu, MorphWind *parent)
{
	_isAlive = true;
	HWND parentHWND;
	if (parent != NULL) {  parentHWND = parent->getHWND();  }
	else {  parentHWND = NULL;  }
	_hwnd = CreateWindowEx(exStyle, className, name, style, dim.left, dim.top, (dim.right - dim.left), (dim.bottom - dim.top)
	  , parentHWND, hmenu, hInstance, this); //This is cool... we store a ptr. to this instance of the class to call the encapsulated wndProc!!!
	if (_hwnd == NULL) {
		std::stringstream ss;
		ss << "Window creation failed with error code: " << GetLastError();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
}


LRESULT MorphWind::sendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return SendMessage(_hwnd, message, wParam, lParam);
}


LRESULT MorphWind::postMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return PostMessage(_hwnd, message, wParam, lParam);
}


bool MorphWind::peekMessage(LPMSG lpMsg, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	return bool(PeekMessage(lpMsg, _hwnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg));
}


BOOL MorphWind::isHwndEqual(HWND hwnd)
{
	return (_hwnd == hwnd);
}


BOOL MorphWind::isWindowVisible() const
{
	return IsWindowVisible(_hwnd);
}


BOOL MorphWind::isMinimized() const
{
	return IsMinimized(_hwnd);
}


BOOL MorphWind::isEnabled() const
{
	return IsWindowEnabled(_hwnd);
}


void MorphWind::invalidate(RECT *rect)
{
	InvalidateRect(_hwnd, rect, false);
}


void MorphWind::update()
{
	UpdateWindow(_hwnd);
}


void MorphWind::redrawWindow(const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags)
{
	RedrawWindow(_hwnd, lprcUpdate, hrgnUpdate, flags);
}


void MorphWind::setFocus()
{
	SetFocus(_hwnd);
}


void MorphWind::setForeground()
{
	SetForegroundWindow(_hwnd);
}


void MorphWind::getDim(LPRECT rect)
{
	if(rect == NULL) {
		debugBreak(DEBUG_LOC, "The argument rect is NULL");
	}
	GetWindowRect(_hwnd, rect);
}


/**
 * Sets the dimensions and placement of the window
 * hInsertAfter ex): HWND_TOPMOST
 * dim ex): some RECT struct
 * flags ex): SWP_NOSIZE or SWP_...
 */
void MorphWind::setDim(HWND hInsertAfter, RECT &dim, UINT flags)
{
	SetWindowPos(_hwnd, hInsertAfter, dim.left, dim.top, (dim.right - dim.left), (dim.bottom - dim.top), flags);
}


void MorphWind::getClientDim(LPRECT rect)
{
	if(rect == NULL) {
		debugBreak(DEBUG_LOC, "The argument rect is NULL");
	}
	GetClientRect(_hwnd, rect);
}


void MorphWind::enable(bool flag)
{
	EnableWindow(_hwnd, BOOL(flag));
}


void MorphWind::showWindow(int nCmdShow)
{
	ShowWindow(_hwnd, nCmdShow);
}


HWND MorphWind::getHWND() const
{
	return _hwnd;
}


HINSTANCE MorphWind::getHINSTANCE() const
{
	return _hInstance;
}


MorphWind& MorphWind::getParent() const
{
	return *_parent;
}


HWND MorphWind::getParentHWND() const
{
	return _parent->getHWND();
}


LRESULT CALLBACK MorphWind::mainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MorphWind *morphWind = (MorphWind*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if(message == WM_NCCREATE) {
		CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
        morphWind = (MorphWind*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)cs->lpCreateParams);
    }
    if(morphWind != NULL) {
		if(message == WM_NCDESTROY)
			SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
		return morphWind->wndProc(hwnd, message, wParam, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


LRESULT CALLBACK MorphWind::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, message, wParam, lParam);
}