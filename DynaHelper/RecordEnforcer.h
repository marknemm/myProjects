#ifndef RECORD_ENFORCER
#define RECORD_ENFORCER
#include <boost/thread/thread.hpp>
#include <Windows.h>
#include <map>
#include <vector>
#include <list>
#include <ctime>
#include <chrono>
#include "ActionMacros.h"
#include "Main.h"
#include "Record.h"


#define K_PRESS_WAIT 20
#define MOUSE_MOVE_WAIT 20
#define RELEASE_WAIT 10
#define SCROLL_WAIT 50
#define D_CLICK_WAIT 250
#define SPEED 15


struct PressEventInfo {
	std::chrono::high_resolution_clock::time_point start;
	int duration;
	int scanCode;
	bool force1Press;
};


/**
 * Enforces recorded mouse event directives.
 * @author Mark Nemmer
 */
class RecordEnforcer {


	static bool _run;
	static std::list<PressEventInfo> _keyPressList;
	//std::map<int,int> _asciiToScan;
	Record *_directive;
	INPUT *_mBuffer;
	int _width;
	int _height;
	int _sd;

	void actionLoop();
	/**
	 * Moves the mouse to the specified location of the directive.
	 * @param x The X-location on the screen to move the mouse.
	 * @param y The Y-location on the screen to move the mouse.
	 */
	void moveMouse(const double &x, const double &y);
	void setMousePos(const int x, const int y);
	void leftClick();
	void leftPress();
	void rightClick();
	void doubleClick();
	void scrollUp();
	void scrollDown();
	void clickAndDrag();
	void textInput();
	void keyType();
	void keyPress();
	void offsetAction();
	void setupAsciiToScan();
	/**
	 * Performs the click/scroll action of the recorded directive.
	 * @param c The character code of the action to be recorded.
	 */
	void enforceAction(const int act);


public:


	/**
	 * Enforces recorded mouse event directives.
	 * @param directive The directive that will be enforced.
	 */
	RecordEnforcer(Record &directive);
	~RecordEnforcer();
	static void holdKeyPresses();
};


#endif