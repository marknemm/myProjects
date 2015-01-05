#include "RecordEnforcer.h"


bool RecordEnforcer::_run;
std::list<PressEventInfo> RecordEnforcer::_keyPressList;


void RecordEnforcer::holdKeyPresses() {
	std::chrono::high_resolution_clock::time_point lastTickTime, curTime;
	std::chrono::milliseconds timeSpan;
	std::list<PressEventInfo>::iterator iter;
	std::vector<INPUT> kBuffer;
	size_t i;
	int topInc(0); // used to make sure we don't go over time
	lastTickTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point timeIn;
	bool timing(false);
	while(_run) {
		std::cout << ""; // Prevents compiler optimizations which would stop thread prematurely
		if(_keyPressList.size() > 0) {
			i = 0;
			iter = std::list<PressEventInfo>::iterator(_keyPressList.begin());
			kBuffer = std::vector<INPUT>(_keyPressList.size());
			// This loop fills the kBuffer and decrements the remaining duration of key events
			while(i < _keyPressList.size() && iter != _keyPressList.end()) {
				kBuffer.at(i).type = INPUT_KEYBOARD;
				kBuffer.at(i).ki.wScan = iter->scanCode;
				// Duration depleted
				timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - iter->start);
				if(((iter->duration - timeSpan.count() + topInc) <= 0) && !iter->force1Press) {
					kBuffer.at(i++).ki.dwFlags = (KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE);
					iter = _keyPressList.erase(iter);
				}
				// Duration not depleted
				else {
					(iter++)->force1Press = false;
					kBuffer.at(i++).ki.dwFlags = KEYEVENTF_SCANCODE;
				}
			}
			do {
				Sleep(1);
				curTime = std::chrono::high_resolution_clock::now();
				timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTickTime);
			} while(timeSpan.count() < K_PRESS_WAIT && _keyPressList.size() > 0);
			lastTickTime = std::chrono::high_resolution_clock::now();
			if(timing && timeSpan.count() > topInc)
				topInc = (int)timeSpan.count();
			else
				timing = true;
			SendInput(i, &kBuffer.at(0), sizeof(INPUT));
		}
		if(timing && _keyPressList.size() == 0)
			_run = false;
	}
}


/** 
* Enforces recorded mouse event directives.
* @param directive The directive that will be enforced.
*/
RecordEnforcer::RecordEnforcer(Record &directive)
: //_asciiToScan(std::map<int,int>()),
  _sd(-1)
{
	_keyPressList = std::list<PressEventInfo>();
	//setupAsciiToScan();
	_directive = &directive;
	RECT desktop;
	HWND hDeskWnd = GetDesktopWindow();
	GetWindowRect(hDeskWnd, &desktop);
	_width = desktop.right;
	_height = desktop.bottom;
	_mBuffer = new INPUT();
	_mBuffer->type = INPUT_MOUSE;
	_mBuffer->mi.dx = (0 * (0xFFFF / _width));
	_mBuffer->mi.dy = (0 * (0xFFFF / _height));
	_mBuffer->mi.mouseData = 0;
	_mBuffer->mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	_mBuffer->mi.time = 0;
	_mBuffer->mi.dwExtraInfo = 0;
	actionLoop();
}


RecordEnforcer::~RecordEnforcer()
{
	delete(_mBuffer);
}


void RecordEnforcer::actionLoop()
{
	_run = true;
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	boost::thread holdKeyPressesThread(RecordEnforcer::holdKeyPresses);
	while(++_sd < _directive->numOfSubDirectives()) {
		startTime = std::chrono::high_resolution_clock::now();
		if((_keyPressList.size() != 0) && !(_directive->getAction(_sd) & KEY)) {
			holdKeyPressesThread.join();
			_run = true;
			holdKeyPressesThread = boost::thread(RecordEnforcer::holdKeyPresses);
		}
		if(!_run) {
			_run = true;
			holdKeyPressesThread = boost::thread(RecordEnforcer::holdKeyPresses);
		}
		do {
			curTime = std::chrono::high_resolution_clock::now();
			Sleep(1);
		} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < _directive->getDelay(_sd));
		if(_directive->getAction(_sd) & FIRST_POINT && _directive->getAction(_sd) != REL_MOUSE_ACT) {
			moveMouse(_directive->getLocation(_sd).x, _directive->getLocation(_sd).y);
			Sleep(5);
		}
		enforceAction(_directive->getAction(_sd));
	}
	if(!_run) {
		_run = true;
		holdKeyPressesThread = boost::thread(RecordEnforcer::holdKeyPresses);
	}
	if(_keyPressList.size() != 0) {
		holdKeyPressesThread.join();
	}
	else {  _run = false;  }
}


/**
* Moves the mouse to the specified location of the directive.
* @param x The X-location on the screen to move the mouse.
* @param y The Y-location on the screen to move the mouse.
*/
void RecordEnforcer::moveMouse(const double &x, const double &y)
{
	RECT res;
	HWND hDesktop(GetDesktopWindow());
	GetClientRect(hDesktop, &res);
	if(x < 0 || y < 0 || x > res.right || y > res.bottom) {
		std::stringstream ss;
		ss << "The point of value (" << int(x) << ", " << int(y) << ") to move the mouse to is outside of the screen resolution range";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	std::chrono::milliseconds duration;
	startTime = std::chrono::high_resolution_clock::now();
	POINT p1;
	GetCursorPos(&p1);
	double nx(p1.x);
	double ny(p1.y);
	if((x - p1.x) != 0) {
		double m = ((double(y) - ny) / (double(x) - nx));
		double b = (ny - (m * nx));
		while((x != int(nx)) || (y != int(ny))) {
			GetCursorPos(&p1);
			//std::stringstream ss;
			//ss << "m: " << m << "		b: " << b << "		nx: " << nx << "		ny: " << ny << "		x: " << x << "		y: " << y << std::endl;
			//OutputDebugString(ss.str().c_str());
			if((m > 1) || (m < -1)) {
				if((y - ny) > 0) { //+y
					if(y - ny > SPEED) {
						ny += SPEED;
						nx = ((ny - b) / m);
					}
					else {
						ny = y;
						nx = x;
					}
				}
				else { //-y
					if((y - ny) < -SPEED) {
						ny -= SPEED;
						nx = ((ny - b) / m);
					}
					else {
						ny = y;
						nx = x;
					}
				}
			}
			else {
				if((x - nx) > 0) { //+x
					if((x - nx) > SPEED) {
						nx += SPEED;
						ny = ((m * nx) + b);
					}
					else {
						nx = x;
						ny = y;
					}
				}
				else { //-x
					if((x - nx) < -SPEED) {
						nx -= SPEED;
						ny = ((m * nx) + b);
					}
					else {
						nx = x;
						ny = y;
					}
				}
			}
			do {
				curTime = std::chrono::high_resolution_clock::now();
				Sleep(1);
			} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < MOUSE_MOVE_WAIT);
			startTime = std::chrono::high_resolution_clock::now();
			setMousePos(int(nx), int(ny));
		}
	}
	else {
		while(y != int(ny)) {
			if((y - ny) > 0) { //+y
				if((y - ny) > SPEED)
					ny += SPEED;
				else
					ny = y;
			}
			else { //-y
				if((y - ny) < -SPEED)
					ny -= SPEED;
				else
					ny = y;
			}
			do {
				curTime = std::chrono::high_resolution_clock::now();
				Sleep(1);
			} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < MOUSE_MOVE_WAIT);
			startTime = std::chrono::high_resolution_clock::now();
			setMousePos(int(nx), int(ny));
		}
	}
}


void RecordEnforcer::setMousePos(const int x, const int y)
{
	_mBuffer->mi.dwFlags = (MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE);
	RECT dims;
	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &dims);
	_mBuffer->mi.dx = (x * 0xFFFF / dims.right + 1);
	_mBuffer->mi.dy = (y * 0xFFFF / dims.bottom);
	SendInput(1, _mBuffer, sizeof(INPUT));
}


void RecordEnforcer::leftClick()
{
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, _mBuffer, sizeof(INPUT));
	_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTUP;
	do {
		curTime = std::chrono::high_resolution_clock::now();
	} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < RELEASE_WAIT);
	SendInput(1, _mBuffer, sizeof(INPUT));
}


void RecordEnforcer::leftPress()
{
	int duration;
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	if(_directive->getPressDuration(_sd) > 30000)
		duration = 30000;
	else
		duration = _directive->getPressDuration(_sd);
	_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, _mBuffer, sizeof(INPUT));
	_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTUP;
	do {
		curTime = std::chrono::high_resolution_clock::now();
		Sleep(1);
	} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < duration);
	SendInput(1, _mBuffer, sizeof(INPUT));
}


void RecordEnforcer::rightClick()
{
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	_mBuffer->mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, _mBuffer, sizeof(INPUT));
	_mBuffer->mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	do {
		curTime = std::chrono::high_resolution_clock::now();
	} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < RELEASE_WAIT);
	SendInput(1, _mBuffer, sizeof(INPUT));
}


void RecordEnforcer::doubleClick()
{
	int duration;
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	for(int i(0) ; i < 2 ; i++) {
		_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, _mBuffer, sizeof(INPUT));
		_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, _mBuffer, sizeof(INPUT));
		if(i == 0)
			duration = D_CLICK_WAIT;
		else
			duration = RELEASE_WAIT;
		do {
			curTime = std::chrono::high_resolution_clock::now();
			if(i == 0)
				Sleep(1);
		} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < duration);
		startTime = std::chrono::high_resolution_clock::now();
	}
}


void RecordEnforcer::scrollUp()
{
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	int numTimes(_directive->getPressDuration(_sd));
	if(numTimes > 100)
		numTimes = 100;
	for(int i(0) ; i < numTimes ; i++) {
		_mBuffer->mi.dwFlags = MOUSEEVENTF_WHEEL;
		_mBuffer->mi.mouseData = 125;
		SendInput(1, _mBuffer, sizeof(INPUT));
		do {
		curTime = std::chrono::high_resolution_clock::now();
		Sleep(1);
		} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < SCROLL_WAIT);
		startTime = std::chrono::high_resolution_clock::now();
	}
}


void RecordEnforcer::scrollDown()
{
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	int numTimes(_directive->getPressDuration(_sd));
	if(numTimes > 100)
		numTimes = 100;
	for(int i(0) ; i < numTimes ; i++) {
		_mBuffer->mi.dwFlags = MOUSEEVENTF_WHEEL;
		_mBuffer->mi.mouseData = -125;
		SendInput(1, _mBuffer, sizeof(INPUT));
		do {
			curTime = std::chrono::high_resolution_clock::now();
			Sleep(1);
		} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < SCROLL_WAIT);
		startTime = std::chrono::high_resolution_clock::now();
	}
}


void RecordEnforcer::clickAndDrag()
{
	std::chrono::high_resolution_clock::time_point startTime, curTime;
	startTime = std::chrono::high_resolution_clock::now();
	_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, _mBuffer, sizeof(INPUT));
	do {
		curTime = std::chrono::high_resolution_clock::now();
		Sleep(1);
	} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < RELEASE_WAIT);
	moveMouse(_directive->getLocation2(_sd).x, _directive->getLocation2(_sd).y);
	_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTUP;
	startTime = std::chrono::high_resolution_clock::now();
	do {
		curTime = std::chrono::high_resolution_clock::now();
	} while(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count() < RELEASE_WAIT);
	SendInput(1, _mBuffer, sizeof(INPUT));
}


void RecordEnforcer::textInput()
{
	std::string text(_directive->getText(_sd));
	if(OpenClipboard(NULL)) {
		HGLOBAL clipbuffer;
		char* buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.length() + 1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, text.length() + 1, text.c_str());
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
	INPUT input[4];
	for(int i(0) ; i < 4 ; i++) {
		input[i].type = INPUT_KEYBOARD;
		input[i].ki.time = 0;
		input[i].ki.wVk = 0;
		input[i].ki.dwExtraInfo = 0;
	}
	input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
	input[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
	input[1].ki.dwFlags = KEYEVENTF_SCANCODE;
	input[1].ki.wScan = MapVirtualKey(0x56, MAPVK_VK_TO_VSC);; // 0x2F == 'V'
	input[2].ki.dwFlags = (KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP);
	input[2].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
	input[3].ki.dwFlags = (KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP);
	input[3].ki.wScan = MapVirtualKey(0x56, MAPVK_VK_TO_VSC);;
	SendInput(4, &input[0], sizeof(INPUT));
}


void RecordEnforcer::keyType()
{
	int scanCode(MapVirtualKey(UINT(_directive->getKeyCode(_sd)), MAPVK_VK_TO_VSC));
	if(scanCode == 0) {
		std::stringstream ss;
		ss << "The key code with value " << _directive->getKeyCode(_sd) << " is not translatable into a scan code";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	PressEventInfo pressInfo = {
		std::chrono::high_resolution_clock::now(),
		0,
		scanCode,
		true
	};
	_keyPressList.push_back(pressInfo);
}


void RecordEnforcer::keyPress()
{
	int scanCode(MapVirtualKey(UINT(_directive->getKeyCode(_sd)), MAPVK_VK_TO_VSC));
	if(scanCode == 0) {
		std::stringstream ss;
		ss << "The key code with value " << _directive->getKeyCode(_sd) << " is not translatable into a scan code";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	int duration(_directive->getPressDuration(_sd));
	if(duration > 3000)
		duration = 3000;
	PressEventInfo pressInfo = {
		std::chrono::high_resolution_clock::now(),
		duration,
		scanCode,
		false
	};
	_keyPressList.push_back(pressInfo);
}


void RecordEnforcer::offsetAction()
{
	POINT position;
	GetCursorPos(&position);
	position.x += _directive->getOffsetDeltaX(_sd);
	position.y += _directive->getOffsetDeltaY(_sd);
	if(_directive->getOffsetAction(_sd) == CLICK_N_DRAG) {
		_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, _mBuffer, sizeof(INPUT));
	}
	moveMouse(position.x, position.y);
	if(_directive->getOffsetAction(_sd) != CLICK_N_DRAG) {
		enforceAction(_directive->getOffsetAction(_sd));
	}
	else {
		_mBuffer->mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, _mBuffer, sizeof(INPUT));
	}
}


/**
* Performs the click/scroll action of the recorded directive.
* @param c The character code of the action to be recorded.
*/
void RecordEnforcer::enforceAction(const int act)
{
	switch(act) {
		case L_CLICK:			leftClick();		break;
		case L_PRESS:			leftPress();		break;
		case R_CLICK:			rightClick();		break;
		case D_CLICK:			doubleClick();		break;
		case SCR_UP:			scrollUp();			break;
		case SCR_DOWN:			scrollDown();		break;
		case CLICK_N_DRAG:		clickAndDrag();		break;
		case TEXT_INP:			textInput();		break;
		case K_TYPE:			keyType();			break;
		case K_PRESS:			keyPress();			break;
		case REL_MOUSE_ACT:		offsetAction();		break;
		case NO_ACT:								break;
		case DEFAULT_ACT:							break;
		default:
			std::stringstream ss;
			ss << "An action with value " << act << " has been passed into the record enforcer and is not defined";
			debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	Sleep(10);
}

/*void RecordEnforcer::setupAsciiToScan()
{	_asciiToScan.insert(std::pair<int,int>('Q', 0x10));
	_asciiToScan.insert(std::pair<int,int>('W', 0x11));
	_asciiToScan.insert(std::pair<int,int>('E', 0x12));
	_asciiToScan.insert(std::pair<int,int>('R', 0x13));
	_asciiToScan.insert(std::pair<int,int>('T', 0x14));
	_asciiToScan.insert(std::pair<int,int>('Y', 0x15));
	_asciiToScan.insert(std::pair<int,int>('U', 0x16));
	_asciiToScan.insert(std::pair<int,int>('I', 0x17));
	_asciiToScan.insert(std::pair<int,int>('O', 0x18));
	_asciiToScan.insert(std::pair<int,int>('P', 0x19));
	_asciiToScan.insert(std::pair<int,int>('[', 0x1A));
	_asciiToScan.insert(std::pair<int,int>('{', 0x1A));
	_asciiToScan.insert(std::pair<int,int>(']', 0x1B));
	_asciiToScan.insert(std::pair<int,int>('}', 0x1B));
	_asciiToScan.insert(std::pair<int,int>('\\',0x2B));
	_asciiToScan.insert(std::pair<int,int>('|', 0x2B));
	_asciiToScan.insert(std::pair<int,int>('A', 0x1E));
	_asciiToScan.insert(std::pair<int,int>('S', 0x1F));
	_asciiToScan.insert(std::pair<int,int>('D', 0x20));
	_asciiToScan.insert(std::pair<int,int>('F', 0x21));
	_asciiToScan.insert(std::pair<int,int>('G', 0x22));
	_asciiToScan.insert(std::pair<int,int>('H', 0x23));
	_asciiToScan.insert(std::pair<int,int>('J', 0x24));
	_asciiToScan.insert(std::pair<int,int>('K', 0x25));
	_asciiToScan.insert(std::pair<int,int>('L', 0x26));
	_asciiToScan.insert(std::pair<int,int>(';', 0x27));
	_asciiToScan.insert(std::pair<int,int>(':', 0x27));
	_asciiToScan.insert(std::pair<int,int>(0x27,0x28));
	_asciiToScan.insert(std::pair<int,int>('"', 0x28));
	_asciiToScan.insert(std::pair<int,int>('Z', 0x2C));
	_asciiToScan.insert(std::pair<int,int>('X', 0x2D));
	_asciiToScan.insert(std::pair<int,int>('C', 0x2E));
	_asciiToScan.insert(std::pair<int,int>('V', 0x2F));
	_asciiToScan.insert(std::pair<int,int>('B', 0x30));
	_asciiToScan.insert(std::pair<int,int>('N', 0x31));
	_asciiToScan.insert(std::pair<int,int>('M', 0x32));
	_asciiToScan.insert(std::pair<int,int>(',', 0x33));
	_asciiToScan.insert(std::pair<int,int>('<', 0x33));
	_asciiToScan.insert(std::pair<int,int>('.', 0x34));
	_asciiToScan.insert(std::pair<int,int>('>', 0x34));
	_asciiToScan.insert(std::pair<int,int>('/', 0x35));
	_asciiToScan.insert(std::pair<int,int>('?', 0x35));
	_asciiToScan.insert(std::pair<int,int>('1', 0x02));
	_asciiToScan.insert(std::pair<int,int>('!', 0x02));
	_asciiToScan.insert(std::pair<int,int>('`', 0x29));
	_asciiToScan.insert(std::pair<int,int>('~', 0x29));
	_asciiToScan.insert(std::pair<int,int>(' ', 0x39));
	_asciiToScan.insert(std::pair<int,int>('2', 0x03));
	_asciiToScan.insert(std::pair<int,int>('@', 0x03));
	_asciiToScan.insert(std::pair<int,int>('3', 0x04));
	_asciiToScan.insert(std::pair<int,int>('#', 0x04));
	_asciiToScan.insert(std::pair<int,int>('4', 0x05));
	_asciiToScan.insert(std::pair<int,int>('$', 0x05));
	_asciiToScan.insert(std::pair<int,int>('5', 0x06));
	_asciiToScan.insert(std::pair<int,int>('%', 0x06));
	_asciiToScan.insert(std::pair<int,int>('6', 0x07));
	_asciiToScan.insert(std::pair<int,int>('^', 0x07));
	_asciiToScan.insert(std::pair<int,int>('7', 0x08));
	_asciiToScan.insert(std::pair<int,int>('&', 0x08));
	_asciiToScan.insert(std::pair<int,int>('8', 0x09));
	_asciiToScan.insert(std::pair<int,int>('*', 0x09));
	_asciiToScan.insert(std::pair<int,int>('9', 0x0A));
	_asciiToScan.insert(std::pair<int,int>('(', 0x0A));
	_asciiToScan.insert(std::pair<int,int>('0', 0x0B));
	_asciiToScan.insert(std::pair<int,int>(')', 0x0B));
	_asciiToScan.insert(std::pair<int,int>('-', 0x0C));
	_asciiToScan.insert(std::pair<int,int>('_', 0x0C));
	_asciiToScan.insert(std::pair<int,int>('=', 0x0D));
	_asciiToScan.insert(std::pair<int,int>('+', 0x0D));
}*/