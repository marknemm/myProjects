#include"DataOptionsh.h"


DataOptions::DataOptions()
: _updateRgnHeader(),
  _updateRgnUpArrow(),
  _updateRgnDownArrow(),
  _exitCmd(CMD_EXIT),
  _changeMade(false),
  _highlightHeader(0),
  _upState(ARROW_NORMAL),
  _downState(ARROW_NORMAL),
  _dataScroll(this),
  _directiveWind(),
  _titleEdit(),
  _delayEdit(),
  _locationEdit(),
  _keyEdit(),
  _durationEdit(),
  _textEdit(),
  _scrollEdit(),
  _recordScreen(NULL),
  _actionDropWind()
{
	_updateRgnHeader.left = 41;
	_updateRgnHeader.top = 62;
	_updateRgnHeader.right = 609;
	_updateRgnHeader.bottom = 85;
	_updateRgnUpArrow.left = 676;
	_updateRgnUpArrow.top = 102;
	_updateRgnUpArrow.right = 726;
	_updateRgnUpArrow.bottom = 152;
	_updateRgnDownArrow.left = 676;
	_updateRgnDownArrow.top = 232;
	_updateRgnDownArrow.right = 726;
	_updateRgnDownArrow.bottom = 282;
}


DataOptions::~DataOptions()
{
	delete(_recordScreen);
}


void DataOptions::initialize(MorphWind *parent)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "Argument *parent is NULL");
	}
	RECT parentClientDim;
	parent->getClientDim(&parentClientDim);
	RECT dim = { 0, 0, parentClientDim.right, parentClientDim.bottom };
	RECT scrollDim = { 40, 85, 610, 290 };
	MorphWind::initialize("Data Options", NULL, _globHINST, NULL, (WS_CHILD | WS_VISIBLE), dim, parent);
	update();
	_dataScroll.initialize("Data Scroll", NULL, _globHINST, NULL, (WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL), scrollDim, this);
	_dataScroll.adjustScrollInfo();
	_directiveWind.initialize(this);
	createButtons();
	createEdits();
	initActionDrop();
	initRelativeActionDrop();
}


bool DataOptions::wasChangeMade() const
{
	return _changeMade;
}


int DataOptions::exitCmd() const
{
	return _exitCmd;
}


std::list<Record*>* DataOptions::getSelectedRecords()
{
	return _dataScroll.getSelectedRecords();
}


int DataOptions::getDirective() const
{
	return _directiveWind.getDirective();
}


void DataOptions::setFieldsEnabled(bool enable, int action)
{
	if(action & FIRST_POINT) {
		_buttons[SET_LOCATION_BUTTON_ID].enable(enable);
	}
	else if(_buttons[SET_LOCATION_BUTTON_ID].isEnabled()) {
		_buttons[SET_LOCATION_BUTTON_ID].enable(false);
	}
	if(action & KEY) {
		_buttons[SET_KEY_BUTTON_ID].enable(enable);
	}
	else if(_buttons[SET_KEY_BUTTON_ID].isEnabled()) {
		_buttons[SET_KEY_BUTTON_ID].enable(false);
	}
	if((action & DURATION) || (action == REL_MOUSE_ACT && (getSelectedRecords()->front()->getOffsetAction(getDirective()) & DURATION))) {
		_buttons[SET_DURATION_BUTTON_ID].enable(enable);
		_durationEdit.enable(enable);
	}
	else if(_buttons[SET_DURATION_BUTTON_ID].isEnabled()) {
		_buttons[SET_DURATION_BUTTON_ID].enable(false);
		_durationEdit.enable(false);
	}
	if(action == TEXT_INP) {
		_buttons[SET_TEXT_BUTTON_ID].enable(enable);
		_textEdit.enable(enable);
	}
	else if(_buttons[SET_TEXT_BUTTON_ID].isEnabled()) {
		_buttons[SET_TEXT_BUTTON_ID].enable(false);
		_textEdit.enable(false);
	}
	if((action & SCROLL) || (action == REL_MOUSE_ACT && (getSelectedRecords()->front()->getOffsetAction(getDirective()) & SCROLL)))
	{
		_buttons[SET_SCROLLS_BUTTON_ID].enable(enable);
		_scrollEdit.enable(enable);
	}
	else if(_buttons[SET_SCROLLS_BUTTON_ID].isEnabled()) {
		_buttons[SET_SCROLLS_BUTTON_ID].enable(false);
		_scrollEdit.enable(false);
	}

	if(_dataScroll._maxDirective > 0) {
		_buttons[DELETE_DIRECTIVE_BUTTON_ID].enable(enable);
	}
	else {  _buttons[DELETE_DIRECTIVE_BUTTON_ID].enable(false);  }

	if(getSelectedRecords() != NULL && getSelectedRecords()->size() == 1) {
		_buttons[SWAP_RECORDS_BUTTON_ID].enable(enable);
	}
	else {  _buttons[SWAP_RECORDS_BUTTON_ID].enable(false);  }

	if(enable == false || _titleEdit.isEnabled() == false) {
		_buttons[SET_TITLE_BUTTON_ID].enable(enable);
		_buttons[SET_DELAY_BUTTON_ID].enable(enable);
		_buttons[DUPLICATE_RECORD_BUTTON_ID].enable(enable);
		_buttons[DELETE_RECORD_BUTTON_ID].enable(enable);
		_buttons[ADD_DIRECTIVE_BUTTON_ID].enable(enable);
		_titleEdit.enable(enable);
		_delayEdit.enable(enable);
		if(enable == false) {
			_titleEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));
			_delayEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));
			_locationEdit[0].sendMessage(WM_SETTEXT, NULL, LPARAM(""));
			_locationEdit[1].sendMessage(WM_SETTEXT, NULL, LPARAM(""));
			_keyEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));
			_textEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));
			_actionDropWind.sendMessage(CB_SETCURSEL, WPARAM(-1), 0);
			_relativeActionDropWind.sendMessage(CB_SETCURSEL, WPARAM(-1), 0);
		}
		_actionDropWind.enable(enable);
	}
	if(action == REL_MOUSE_ACT && enable) {
		_relativeActionDropWind.enable(enable);
	}
	else if(_relativeActionDropWind.isEnabled()) {
		_relativeActionDropWind.enable(false);
		_relativeActionDropWind.sendMessage(CB_SETCURSEL, WPARAM(-1), 0);
	}
}


void DataOptions::selectRecord()
{
	selectDirective(0);
}


int DataOptions::getSelectedAction()
{
	if(_dataScroll._maxDirective < 0) {
		return DEFAULT_ACT;
	}
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		return getSelectedRecords()->front()->getAction(getDirective());
	}
	bool sameAction(true);
	int prevAction(getSelectedRecords()->front()->getAction(getDirective()));
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevAction != (*it)->getAction(getDirective())) {
			sameAction = false;
			break;
		}
		else {  prevAction = (*it)->getAction(getDirective());  }
	}
	if(sameAction) {
		return prevAction;
	}
	return DEFAULT_ACT;
}


int DataOptions::getSelectedRelativeAction()
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		return getSelectedRecords()->front()->getOffsetAction(getDirective());
	}
	int prevAction(getSelectedRecords()->front()->getOffsetAction(getDirective()));
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevAction != (*it)->getOffsetAction(getDirective())) {
			return DEFAULT_ACT;
		}
		else {  prevAction = (*it)->getOffsetAction(getDirective());  }
	}
	return prevAction;
}


void DataOptions::getSelectedTitle(std::string &title)
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		title = getSelectedRecords()->front()->getTitle();
		return;
	}
	std::string prevTitle(getSelectedRecords()->front()->getTitle());
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevTitle.compare((*it)->getTitle()) != 0) {
			title = "";
			return;
		}
		else {  prevTitle = (*it)->getTitle();  }
	}
	title = prevTitle;
}


void DataOptions::getSelectedLocation(Point &loc)
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		loc = getSelectedRecords()->front()->getLocation(getDirective());
		return;
	}
	int prevX(getSelectedRecords()->front()->getLocation(getDirective()).x);
	int prevY(getSelectedRecords()->front()->getLocation(getDirective()).y);
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevX != (*it)->getLocation(getDirective()).x
		|| prevY != (*it)->getLocation(getDirective()).y)
		{
			loc.x = -1; // Return -1 in x to show that there's no common location between selected records.
			return;
		}
		else {
			prevX = (*it)->getLocation(getDirective()).x;
			prevY = (*it)->getLocation(getDirective()).y;
		}
	}
	loc.x = prevX;
	loc.y = prevY;
}


void DataOptions::getSelectedLocation2(Point &loc2)
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		loc2 = getSelectedRecords()->front()->getLocation2(getDirective());
		return;
	}
	int prevX(getSelectedRecords()->front()->getLocation2(getDirective()).x);
	int prevY(getSelectedRecords()->front()->getLocation2(getDirective()).y);
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevX != (*it)->getLocation2(getDirective()).x
		|| prevY != (*it)->getLocation2(getDirective()).y)
		{
			loc2.x = -1; // Return -1 in x to show that there's no common location2 between selected records.
			return;
		}
		else {
			prevX = (*it)->getLocation2(getDirective()).x;
			prevY = (*it)->getLocation2(getDirective()).y;
		}
	}
	loc2.x = prevX;
	loc2.y = prevY;
}


void DataOptions::getSelectedDelay(double &delay)
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		delay = ((double)getSelectedRecords()->front()->getDelay(getDirective()) / 1000.0);
		return;
	}
	double prevDelay(getSelectedRecords()->front()->getDelay(getDirective()));
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevDelay != (*it)->getDelay(getDirective())) {
			delay = -1.0; // Return -1 in x to show that there's no common delay between selected records.
			return;
		}
		else {  prevDelay = (*it)->getDelay(getDirective());  }
	}
	delay = ((double)prevDelay / 1000.0);
}


void DataOptions::getSelectedDuration(double &duration)
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		duration = ((double)getSelectedRecords()->front()->getPressDuration(getDirective()) / 1000.0);
		return;
	}
	double prevDuration(getSelectedRecords()->front()->getPressDuration(getDirective()));
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevDuration != (*it)->getPressDuration(getDirective())) {
			duration = -1.0; // Return -1 in x to show that there's no common duration between selected records.
			return;
		}
		else {  prevDuration = (*it)->getPressDuration(getDirective());  }
	}
	duration = ((double)prevDuration / 1000.0);
}


int DataOptions::getSelectedKey()
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		return getSelectedRecords()->front()->getKeyCode(getDirective());
	}
	int prevKey(getSelectedRecords()->front()->getKeyCode(getDirective()));
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevKey != (*it)->getKeyCode(getDirective())) {
			return -1;
		}
		else {  prevKey = (*it)->getKeyCode(getDirective());  }
	}
	return prevKey;
}


void DataOptions::getSelectedText(std::string &text)
{
	if(_dataScroll.getSelectedRecords()->size() == 1) {
		text = getSelectedRecords()->front()->getText(getDirective());
		return;
	}
	std::string prevText(getSelectedRecords()->front()->getText(getDirective()));
	for(std::list<Record*>::iterator it(++getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
		if(prevText.compare((*it)->getText(getDirective())) != 0) {
			text = "";
			return;
		}
		else {  prevText = (*it)->getText(getDirective());  }
	}
	text = prevText;
}


void DataOptions::getStringOfDouble(std::stringstream &ss, double &doub)
{
	int thousands = int(doub / 1000);
	int hundreds = int((doub - (thousands * 1000)) / 100);
	int tens = int((doub - (thousands * 1000) - (hundreds * 100)) / 10);
	int ones = (int(doub) % 10);
	int tenths = int(std::abs(doub - int(doub)) * 10);
	int hundredths = (int(std::abs(doub - int(doub)) * 100) - (tenths*10));
	int thousandths = (int(std::abs(doub - int(doub)) * 1000) - (hundredths*10) - (tenths*100));
	if(thousands > 0) {
		ss << thousands;
		OutputDebugString(ss.str().c_str());
	}
	if((hundreds > 0) || (thousands > 0)) {
		ss << hundreds;
	}
	if((tens > 0) || (hundreds > 0) || (thousands > 0)) {
		ss << tens;
	}
	ss << ones;
	if((tenths > 0) || (hundredths > 0) || (thousandths > 0)) {
		ss << '.' << tenths;
		if((hundredths > 0) || (thousandths > 0)) {
			ss << hundredths;
			if(thousandths > 0) {
				ss << thousandths;
			}
		}
	}
}


void DataOptions::selectDirective(int directive)
{
	_directiveWind.setDirective(directive);

	if(getSelectedRecords() != NULL && _dataScroll._maxDirective >= 0) {
		bool size1(getSelectedRecords()->size() == 1);
		int curAction(getSelectedAction());
		setFieldsEnabled(true, curAction);

		std::string title("");
		getSelectedTitle(title);
		_titleEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(title.c_str()));

		std::stringstream ss("");
		bool lowerDelayOk(false);
		double doub;
		getSelectedDelay(doub);
		if(doub >= 0.0) {
			getStringOfDouble(ss, doub);
			bool lowerDelayOk = (getDirective() > 0 && getSelectedRecords()->front()->getAction(getDirective() - 1) == K_PRESS);
		}
		_delayEdit.sendMessage(WM_SETTEXT, WPARAM(lowerDelayOk), LPARAM(ss.str().c_str()));

		if(curAction & FIRST_POINT) {
			Point loc;
			getSelectedLocation(loc);
			if(loc.x >= 0) {
				ss = std::stringstream();
				ss << "p1: (" << getSelectedRecords()->front()->getLocation(getDirective()).x;
				ss << ", " << getSelectedRecords()->front()->getLocation(getDirective()).y << ")";
				_locationEdit[0].sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
				if(curAction & SECOND_POINT) {
					Point loc2;
					getSelectedLocation2(loc2);
					if(loc2.x >= 0) {
						ss = std::stringstream();
						ss << "p2: (" << getSelectedRecords()->front()->getLocation2(getDirective()).x;
						ss << ", " << getSelectedRecords()->front()->getLocation2(getDirective()).y << ")";
						_locationEdit[1].sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
					}
					else {  _locationEdit[1].sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
				}
				else {  _locationEdit[1].sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
			}
			else {  _locationEdit[0].sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
		}
		else {  _locationEdit[0].sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }

		if(curAction == TEXT_INP) {
			std::string text;
			getSelectedText(text);
			_textEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(text.c_str()));
		}
		else {  _textEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }

		if(curAction & KEY) {
			int key(getSelectedKey());
			if(key > 0) {
				_keyEdit.setText(getSelectedRecords()->front()->getKeyCode(getDirective()));
			}
			else {  _keyEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
		}
		else {  _keyEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }

		if(curAction & DURATION) {
			double doub;
			getSelectedDuration(doub);
			if(doub >= 0.0) {
				ss = std::stringstream();
				getStringOfDouble(ss, doub);
				_durationEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
			}
			else {  _durationEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
		}
		else {  _durationEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }

		if((curAction & SCROLL) || (curAction == REL_MOUSE_ACT && (getSelectedRecords()->front()->getOffsetAction(getDirective()) & SCROLL))) {
			double doub;
			getSelectedDuration(doub);
			if(doub >= 0.0) {
				ss = std::stringstream();
				ss << getSelectedRecords()->front()->getPressDuration(getDirective());
				_scrollEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
			}
			else {  _scrollEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
		}
		else {  _scrollEdit.sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }

		std::string action("");
		switch(curAction) {
			case L_CLICK:			action += "Left Click";				break;
			case L_PRESS:			action += "Left Press";				break;
			case R_CLICK:			action += "Right Click";			break;
			case D_CLICK:			action += "Double Click";			break;
			case SCR_UP:			action += "Scroll Up";				break;
			case SCR_DOWN:			action += "Scroll Down";			break;
			case CLICK_N_DRAG:		action += "Click & Drag";			break;
			case TEXT_INP:			action += "Text Input";				break;
			case K_TYPE:			action += "Key Type";				break;
			case K_PRESS:			action += "Key Press";				break;
			case REL_MOUSE_ACT:		action += "Relative Mouse Action";	break;
			case NO_ACT:			action += "No Action";				break;
			default:				_actionDropWind.sendMessage(CB_SETCURSEL, WPARAM(-1), 0);
		}
		if(action.size() > 0) {
			_actionDropWind.sendMessage(CB_SELECTSTRING, WPARAM(-1), LPARAM(action.c_str()));
		}

		if(curAction == REL_MOUSE_ACT) {
			action = "";
			int relAction(getSelectedRelativeAction());
			switch(relAction) {
				case L_CLICK:			action += "Rel. Left Click";	break;
				case L_PRESS:			action += "Rel. Left Press";	break;
				case R_CLICK:			action += "Rel. Right Click";	break;
				case D_CLICK:			action += "Rel. Double Click";	break;
				case SCR_UP:			action += "Rel. Scroll Up";		break;
				case SCR_DOWN:			action += "Rel. Scroll Down";	break;
				case CLICK_N_DRAG:		action += "Rel. Click & Drag";	break;
				case NO_ACT:			action += "Rel. No Action";		break;
				default:				_relativeActionDropWind.sendMessage(CB_SETCURSEL, WPARAM(-1), 0);
			}
			if(action.size() > 0) {
				_relativeActionDropWind.sendMessage(CB_SELECTSTRING, WPARAM(-1), LPARAM(action.c_str()));
			}
		}

		_dataScroll.setFocus();
	}
	else if(_titleEdit.isEnabled()) {  setFieldsEnabled(false, DEFAULT_ACT);  }
}


void DataOptions::drawScrollHeader(Gdiplus::Graphics &graphics, RECT &rcClip)
{
	using namespace Gdiplus;
	if(!(rcClip.left > _updateRgnHeader.right || rcClip.right < _updateRgnHeader.left || rcClip.top > _updateRgnHeader.bottom || rcClip.bottom < _updateRgnHeader.top)) {
		StringFormat sform;
		sform.SetAlignment(StringAlignmentCenter);
		Pen linePen(Gdiplus::Color(100, 100, 100), REAL(1.0F));
		SolidBrush textBrush(Color(0, 0, 0));
		SolidBrush highlightBrush(Color(75, 255, 75));
		Font font(L"Ariel", 17);
		graphics.FillRectangle(&SolidBrush(Color(160, 220, 40)), (_updateRgnHeader.left - rcClip.left)
		  , (_updateRgnHeader.top - rcClip.top), (_updateRgnHeader.right - _updateRgnHeader.left), (_updateRgnHeader.bottom - _updateRgnHeader.top));
		switch(_highlightHeader) {
			case INDEX_HEADER:		graphics.FillRectangle(&highlightBrush, (41 - rcClip.left), (62 - rcClip.top), 80, 24);		break;
			case ALPHA_HEADER:		graphics.FillRectangle(&highlightBrush, (122 - rcClip.left), (62 - rcClip.top), 249, 24);	break;
			case TIME_HEADER:		graphics.FillRectangle(&highlightBrush, (372 - rcClip.left), (62 - rcClip.top), 237, 24);	break;
		}
		graphics.DrawLine(&linePen, Gdiplus::Point((40 - rcClip.left), (61 - rcClip.top)), Gdiplus::Point((40 - rcClip.left), (85 - rcClip.top)));
		graphics.DrawLine(&linePen, Gdiplus::Point((40 - rcClip.left), (61 - rcClip.top)), Gdiplus::Point((609 - rcClip.left), (61 - rcClip.top)));
		graphics.DrawLine(&linePen, Gdiplus::Point((609 - rcClip.left), (61 - rcClip.top)), Gdiplus::Point((609 - rcClip.left), (85 - rcClip.top)));
		graphics.DrawLine(&linePen, Gdiplus::Point((121 - rcClip.left), (61 - rcClip.top)), Gdiplus::Point((121 - rcClip.left), (85 - rcClip.top)));
		graphics.DrawLine(&linePen, Gdiplus::Point((371 - rcClip.left), (61 - rcClip.top)), Gdiplus::Point((371 - rcClip.left), (85 - rcClip.top)));
		graphics.DrawString(L"Indicees", 8, &font, RectF(REAL(40 - rcClip.left), REAL(59 - rcClip.top), 81, 24), &sform, &textBrush);
		graphics.DrawString(L"Record Titles", 13, &font, RectF(REAL(121 - rcClip.left), REAL(59 - rcClip.top), 250, 24), &sform, &textBrush);
		graphics.DrawString(L"Dates & Times", 13, &font, RectF(REAL(371 - rcClip.left), REAL(59 - rcClip.top), 238, 24), &sform, &textBrush);
	}
}


void DataOptions::drawDirective(Gdiplus::Graphics &graphics, RECT &rcClip, Gdiplus::StringFormat &stringform)
{
	using namespace Gdiplus;
	RectF directiveRECT(REAL(625), REAL(70), REAL(233), REAL(216));
	if(!(directiveRECT.X > rcClip.right || directiveRECT.GetRight() < rcClip.left || directiveRECT.Y > rcClip.bottom || directiveRECT.GetBottom() < rcClip.top)) {
		directiveRECT.X -= rcClip.left;
		directiveRECT.Y -= rcClip.top;
		Image *upArrow, *downArrow;
		Color black(0, 0, 0);
		graphics.DrawRectangle(&Pen(black), directiveRECT);
		if(rcClip.top < 100) {
			Font directiveFont(L"Ariel", REAL(20), FontStyleBold, UnitPoint, NULL);
			graphics.DrawString(L"Directive", 9, &directiveFont, directiveRECT, &stringform, &SolidBrush(black));
		}
		if(!(_updateRgnUpArrow.left > rcClip.right || _updateRgnUpArrow.right < rcClip.left
		|| _updateRgnUpArrow.top > rcClip.bottom || _updateRgnUpArrow.bottom < rcClip.top))
		{
			switch(_upState) {
				case ARROW_NORMAL:	upArrow = new Image(L"UpArrow.png");			break;
				case ARROW_HOVER:	upArrow = new Image(L"UpArrowHover.png");		break;
				case ARROW_PRESS:	upArrow = new Image(L"UpArrowPushed.png");		break;
			}
			graphics.DrawImage(upArrow, (676 - rcClip.left), (102 - rcClip.top));
			delete(upArrow);
		}
		if(!(_updateRgnDownArrow.left > rcClip.right || _updateRgnDownArrow.right < rcClip.left
		|| _updateRgnDownArrow.top > rcClip.bottom || _updateRgnDownArrow.bottom < rcClip.top))
		{
			switch(_downState) {
				case ARROW_NORMAL:	downArrow = new Image(L"DownArrow.png");		break;
				case ARROW_HOVER:	downArrow = new Image(L"DownArrowHover.png");	break;
				case ARROW_PRESS:	downArrow = new Image(L"DownArrowPushed.png");	break;
			}
			graphics.DrawImage(downArrow, (676 - rcClip.left), (232 - rcClip.top));
			delete(downArrow);
		}
	}
}


void DataOptions::paint()
{
	using namespace Gdiplus;
	if(GetUpdateRect(getHWND(), NULL, FALSE) != 0) {
		HDC hdc, hdcBuffer;
		HBITMAP hBitmap;
		RECT rcClient, rcClip;
		PAINTSTRUCT ps;
	
		StringFormat stringform;
		Color black(0, 0, 0);
	
		getClientDim(&rcClient);
		hdc = BeginPaint(getHWND(), &ps);
		GetClipBox(hdc, &rcClip);
		hdcBuffer = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
		SelectObject(hdcBuffer, hBitmap);
		Graphics graphics(hdcBuffer);
		stringform.SetAlignment(StringAlignmentCenter);
	
		graphics.FillRectangle(&SolidBrush(Color(255, 255, 255)), 0, 0, (rcClip.right - rcClip.left), (rcClient.bottom - rcClip.top));
		Gdiplus::LinearGradientBrush background(Gdiplus::Rect((10 - rcClip.left), (10 - rcClip.top), (rcClient.right - 20), (rcClient.bottom - 20))
		  , Gdiplus::Color(200, 220, 0), Gdiplus::Color(0, 200, 100), Gdiplus::REAL(90), FALSE);
		if(!(rcClip.right < 20 || rcClip.left > 220 || rcClip.bottom < 13 || rcClip.top > 41)) {
			graphics.FillRectangle(&background, (20 - rcClip.left), (13 - rcClip.top), 200, 28);
			RectF strBoundRectF(REAL(20 - rcClip.left), REAL(13 - rcClip.top), REAL(200), REAL(28));
			graphics.DrawString(L"Data Options", 12, &Font(L"Ariel", 17), strBoundRectF, &stringform, &SolidBrush(black));
		}

		graphics.FillRectangle(&background, (10 - rcClip.left), (40 - rcClip.top), (rcClient.right - 20), (rcClient.bottom - 50));
		drawScrollHeader(graphics, rcClip);
		drawDirective(graphics, rcClip, stringform);
		if(_dataScroll.isSwapModeEnabled() && !(rcClip.right < 40 || rcClip.left > 200 || rcClip.bottom < 44 || rcClip.top > 60)) {
			graphics.DrawString(L"Swap Mode Enabled", 17, &Font(L"Times New Roman", 10), PointF(REAL(40 - rcClip.left), REAL(45 - rcClip.top)), &SolidBrush(Color(150, 150, 150)));
		}
		
		BitBlt(hdc, rcClip.left, rcClip.top, rcClip.right, rcClip.bottom, hdcBuffer, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
		DeleteObject(hdcBuffer);
		EndPaint(getHWND(), &ps);
	}
}

void DataOptions::mouseMove(LPARAM lParam)
{
	int x, y;
	x = GET_X_LPARAM(lParam);
	y = GET_Y_LPARAM(lParam);
	bool inHeader1 = (x < 121 && y < 85 && x > 40 && y > 61);
	bool inHeader2 = (x < 371 && y < 85 && x > 121 && y > 61);
	bool inHeader3 = (x < 609 && y < 85 && x > 371 && y > 61);
	bool inUpArrow = (x <= 726 && y <= 152 && x >= 676 && y >= 102);
	bool inDownArrow = (x <= 726 && y <= 282 && x >= 676 && y >= 232);

	if(inHeader1 && _highlightHeader != 1) {
		_highlightHeader = INDEX_HEADER;
		redrawWindow(&_updateRgnHeader, NULL, RDW_INVALIDATE);
	}
	else if(inHeader2) {
		_highlightHeader = ALPHA_HEADER;
		redrawWindow(&_updateRgnHeader, NULL, RDW_INVALIDATE);
	}
	else if(inHeader3) {
		_highlightHeader = TIME_HEADER;
		redrawWindow(&_updateRgnHeader, NULL, RDW_INVALIDATE);
	}
	else if(!inHeader1 && !inHeader2 && !inHeader3 && _highlightHeader != 0) {
		_highlightHeader = 0;
		redrawWindow(&_updateRgnHeader, NULL, RDW_INVALIDATE);
	}

	if(inUpArrow && _upState == ARROW_NORMAL && getSelectedRecords() != NULL && (getDirective() + 1) <= _dataScroll._maxDirective) {
		_upState = ARROW_HOVER;
		redrawWindow(&_updateRgnUpArrow, NULL, RDW_INVALIDATE);
	}
	else if(inDownArrow && _downState == ARROW_NORMAL && getSelectedRecords() != NULL && getDirective() != 0) {
		_downState = ARROW_HOVER;
		redrawWindow(&_updateRgnDownArrow, NULL, RDW_INVALIDATE);
	}
	else if(!inDownArrow && _downState != ARROW_NORMAL) {
		_downState = ARROW_NORMAL;
		redrawWindow(&_updateRgnDownArrow, NULL, RDW_INVALIDATE);
	}
	else if(!inUpArrow && _upState != ARROW_NORMAL) {
		_upState = ARROW_NORMAL;
		redrawWindow(&_updateRgnUpArrow, NULL, RDW_INVALIDATE);
	}
}


void DataOptions::leftButtonDown()
{
	switch(_highlightHeader) {
		case INDEX_HEADER:
			_dataScroll.orderByIndex();
			selectRecord();
			break;
		case ALPHA_HEADER:
			_dataScroll.orderByAlpha();
			selectRecord();
			break;
		case TIME_HEADER:
			_dataScroll.orderByTime();
			selectRecord();
			break;
	}
	if(_downState == ARROW_HOVER) {
		_downState = ARROW_PRESS;
		if((_directiveWind.getDirective() - 1) >= 0) {
			selectDirective(_directiveWind.getDirective() - 1);
		}
		redrawWindow(&_updateRgnDownArrow, NULL, RDW_INVALIDATE);
	}
	else if(_upState == ARROW_HOVER) {
		_upState = ARROW_PRESS;
		if((_directiveWind.getDirective() + 1) <= _dataScroll._maxDirective) {
			selectDirective(_directiveWind.getDirective() + 1);
		}
		redrawWindow(&_updateRgnUpArrow, NULL, RDW_INVALIDATE);
	}
}


void DataOptions::leftButtonUp()
{
	if(_downState == ARROW_PRESS) {
		if(getDirective() != 0) {
			_downState = ARROW_HOVER;
		}
		else {  _downState = ARROW_NORMAL;  }
		redrawWindow(&_updateRgnDownArrow, NULL, RDW_INVALIDATE);
	}
	else if(_upState == ARROW_PRESS) {
		if((getDirective() + 1) <= _dataScroll._maxDirective) {
			_upState = ARROW_HOVER;
		}
		else {  _upState = ARROW_NORMAL;  }
		redrawWindow(&_updateRgnUpArrow, NULL, RDW_INVALIDATE);
	}
}


void DataOptions::mouseWheel(WPARAM wParam, LPARAM lParam)
{
	_dataScroll.sendMessage(WM_MOUSEWHEEL, wParam, lParam);
}


void DataOptions::command(WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	switch(wParam) {
		case SET_TITLE_BUTTON_ID:
			if(_titleEdit.isInputValid()) {
				_changeMade = true;
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					(*it)->setTitle(_titleEdit.getTitle());
				}
				_dataScroll.getClientDim(&rcClient);
				_dataScroll.redrawWindow(&rcClient, NULL, RDW_INVALIDATE);
				_dataScroll.setFocus();
			}
			break;
		case SET_DELAY_BUTTON_ID:
			if(_delayEdit.isInputValid()) {
				_changeMade = true;
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					(*it)->setDelay(getDirective(), _delayEdit.getDelay());
				}
				_dataScroll.setFocus();
			}
			break;
		case SET_LOCATION_BUTTON_ID:
			{
				_changeMade = true;
				delete(_recordScreen);
				getParent().showWindow(SW_MINIMIZE);
				Sleep(250);
				getParent().showWindow(SW_HIDE);
				_recordScreen = new RecordScreen(false);
				_recordScreen->waitForInput(getSelectedRecords()->front()->getAction(getDirective()));
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					(*it)->setLocation(getDirective(), _recordScreen->locationEntered());
				}
				std::stringstream ss = std::stringstream();
				ss << "p1: (" << getSelectedRecords()->front()->getLocation(getDirective()).x;
				ss << ", " << getSelectedRecords()->front()->getLocation(getDirective()).y << ")";
				_locationEdit[0].sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
				if(getSelectedRecords()->front()->getAction(getDirective()) & SECOND_POINT) {
					if(getSelectedRecords()->front()->getAction(getDirective()) == REL_MOUSE_ACT) {
						for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
							(*it)->setLocation2(getDirective(), _recordScreen->location2Entered());
							(*it)->setOffsetDeltaX(getDirective(), _recordScreen->location2Entered().x - _recordScreen->locationEntered().x);
							(*it)->setOffsetDeltaY(getDirective(), _recordScreen->location2Entered().y - _recordScreen->locationEntered().y);
						}
					}
					else {
						for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
							(*it)->setLocation2(getDirective(), _recordScreen->location2Entered());
						}
					}
					std::stringstream ss = std::stringstream();
					ss << "p2: (" << getSelectedRecords()->front()->getLocation2(getDirective()).x;
					ss << ", " << getSelectedRecords()->front()->getLocation2(getDirective()).y << ")";
					_locationEdit[1].sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
				}
				else {  _locationEdit[1].sendMessage(WM_SETTEXT, NULL, LPARAM(""));  }
				getParent().showWindow(SW_MINIMIZE);
				getParent().showWindow(SW_RESTORE);
				_dataScroll.setFocus();
			}
			break;
		case SET_KEY_BUTTON_ID:
			_changeMade = true;
			if(_recordScreen != NULL) {
				delete(_recordScreen);
			}
			_recordScreen = new RecordScreen(false);
			_recordScreen->waitForInput(getSelectedRecords()->front()->getAction(getDirective()));
			for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
				(*it)->setKeyCode(getDirective(), _recordScreen->keyEntered());
			}
			_keyEdit.setText(_recordScreen->keyEntered());
			_dataScroll.setFocus();
			break;
		case SET_DURATION_BUTTON_ID:
			if(_durationEdit.isInputValid()) {
				_changeMade = true;
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					(*it)->setPressDuration(getDirective(), _durationEdit.getDelay());
				}
				_dataScroll.setFocus();
			}
			break;
		case SET_TEXT_BUTTON_ID:
			if(_textEdit.isTextValid()) {
				_changeMade = true;
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					(*it)->setText(getDirective(), _textEdit.getText());
				}
				_dataScroll.setFocus();
			}
			break;
		case SET_SCROLLS_BUTTON_ID:
			if(_scrollEdit.isInputValid()) {
				_changeMade = true;
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					(*it)->setPressDuration(getDirective(), _scrollEdit.getNumScrolls());
				}
				_dataScroll.setFocus();
			}
			break;
		case DUPLICATE_RECORD_BUTTON_ID:
			{
				_changeMade = true;
				std::list<Record*> newRecs;
				for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
					newRecs.push_back(_recordHandler->duplicateRecord((*it)->numIndex));
				}
				_dataScroll.adjustScrollInfo();
				_dataScroll.selectAndScrollToRecords(newRecs);
				_dataScroll.setFocus();
			}
			break;
		case ADD_RECORD_BUTTON_ID:
			{
				_changeMade = true;
				std::list<Record*> newRec;
				newRec.push_back(_recordHandler->addDefaultRecord());
				_dataScroll.adjustScrollInfo();
				_dataScroll.selectAndScrollToRecords(newRec);
				_dataScroll.setFocus();
			}
			break;
		case ADD_DIRECTIVE_BUTTON_ID:
			_changeMade = true;
			for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
				(*it)->addDefaultDirective(getDirective() + 1);
			}
			_dataScroll._maxDirective += 1;
			selectDirective(getDirective() + 1);
			_dataScroll.setFocus();
			break;
		case SWAP_RECORDS_BUTTON_ID:
			{
				_changeMade = true;
				_dataScroll.enableSwapMode(!_dataScroll.isSwapModeEnabled());
				RECT updateRgn = { 40, 44, 200, 60 };
				redrawWindow(&updateRgn, NULL, RDW_INVALIDATE);
				_dataScroll.setFocus();
			}
			break;
		case DELETE_RECORD_BUTTON_ID:
			_changeMade = true;
			for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
				_recordHandler->deleteRecord((*it)->numIndex);
			}
			_dataScroll._maxDirective = -1;
			_dataScroll.getClientDim(&rcClient);
			_dataScroll.redrawWindow(&rcClient, NULL, RDW_INVALIDATE);
			selectDirective(0);
			_dataScroll.setFocus();
			break;
		case DELETE_DIRECTIVE_BUTTON_ID:
			_changeMade = true;
			for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
				if((*it)->numOfSubDirectives() != 1) {
					(*it)->deleteDirective(getDirective());
				}
			}
			_dataScroll._maxDirective -= 1;
			if(_dataScroll._maxDirective < getDirective()) {
				selectDirective(getDirective() - 1);
			}
			else {  selectDirective(getDirective());  }
			_dataScroll.setFocus();
			break;
		case DELETE_ALl_RECORDS_BUTTON_ID:
			_changeMade = true;
			setFieldsEnabled(false, DEFAULT_ACT);
			_dataScroll._selectedInd.clear();
			_recordHandler->dumpRecords();
			_dataScroll._highlightInd = -1;
			_dataScroll.adjustScrollInfo();
			_dataScroll.getClientDim(&rcClient);
			_dataScroll.redrawWindow(&rcClient, NULL, RDW_INVALIDATE);
			break;
		case SAVE_AND_EXIT_BUTTON_ID:
			_exitCmd = CMD_EXIT_AND_SAVE;
			getParent().sendMessage(WM_CLOSE, NULL, NULL);
			_exitCmd = CMD_EXIT;
			break;
		case EXIT_BUTTON_ID:
			getParent().sendMessage(WM_CLOSE, NULL, NULL);
			break;
		default:
			if(HIWORD(wParam) == CBN_SELCHANGE) {
				_changeMade = true;
				int action(indexToAction(SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL)));
				if(getSelectedRecords()->size() > 0) {
					if(_actionDropWind.isHwndEqual((HWND)lParam)) {
						for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
							(*it)->setAction(getDirective(), action);
						}
						selectDirective(getDirective());
					}
					else {
						if(action == TEXT_INP) {
							action = NO_ACT;
						}
						for(std::list<Record*>::iterator it(getSelectedRecords()->begin()) ; it != getSelectedRecords()->end() ; it++) {
							(*it)->setOffsetAction(getDirective(), action);
						}
						selectDirective(getDirective());
					}
				}
				_dataScroll.setFocus();
			}
	} // END CASE
}


void DataOptions::appMsg(UINT message)
{
	int sleepAmt(175);
	switch(message - WM_APP) {
		case SET_TITLE_BUTTON_ID:
			_buttons[SET_TITLE_BUTTON_ID].sendMessage(WM_LBUTTONDOWN, NULL, NULL);
			Sleep(sleepAmt);
			_buttons[SET_TITLE_BUTTON_ID].sendMessage(WM_LBUTTONUP, NULL, NULL);
			break;
		case SET_DELAY_BUTTON_ID:
			_buttons[SET_DELAY_BUTTON_ID].sendMessage(WM_LBUTTONDOWN, NULL, NULL);
			Sleep(sleepAmt);
			_buttons[SET_DELAY_BUTTON_ID].sendMessage(WM_LBUTTONUP, NULL, NULL);
			break;
		case SET_DURATION_BUTTON_ID:
			_buttons[SET_DURATION_BUTTON_ID].sendMessage(WM_LBUTTONDOWN, NULL, NULL);
			Sleep(sleepAmt);
			_buttons[SET_DURATION_BUTTON_ID].sendMessage(WM_LBUTTONUP, NULL, NULL);
			break;
		case SET_TEXT_BUTTON_ID:
			_buttons[SET_TEXT_BUTTON_ID].sendMessage(WM_LBUTTONDOWN, NULL, NULL);
			Sleep(sleepAmt);
			_buttons[SET_TEXT_BUTTON_ID].sendMessage(WM_LBUTTONUP, NULL, NULL);
			break;
		case SET_SCROLLS_BUTTON_ID:
			_buttons[SET_SCROLLS_BUTTON_ID].sendMessage(WM_LBUTTONDOWN, NULL, NULL);
			Sleep(sleepAmt);
			_buttons[SET_SCROLLS_BUTTON_ID].sendMessage(WM_LBUTTONUP, NULL, NULL);
			break;
		case DELETE_RECORD_BUTTON_ID:
			if(_buttons[DELETE_RECORD_BUTTON_ID].isEnabled()) {
				_buttons[DELETE_RECORD_BUTTON_ID].sendMessage(WM_LBUTTONDOWN, NULL, NULL);
				Sleep(sleepAmt);
				_buttons[DELETE_RECORD_BUTTON_ID].sendMessage(WM_LBUTTONUP, NULL, NULL);
			}
			break;
	}
}


void DataOptions::keyDown(WPARAM wParam)
{
	if(wParam == VK_ESCAPE) {
		getParent().sendMessage(WM_CLOSE, NULL, NULL);
	}
	else {  _dataScroll.sendMessage(WM_KEYDOWN, wParam, NULL);  }
}


LRESULT CALLBACK DataOptions::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_MOUSEMOVE:		mouseMove(lParam);			break;
		case WM_LBUTTONDOWN:	leftButtonDown();			break;
		case WM_LBUTTONUP:		leftButtonUp();				break;
		case WM_MOUSEWHEEL:		mouseWheel(wParam, lParam);	break;
		case WM_PAINT:			paint();					break;
		case WM_COMMAND:		command(wParam, lParam);	break;
		case WM_KEYDOWN:		keyDown(wParam);			break;
		case WM_ERASEBKGND:									return 0;
		default:
			if(message >= WM_APP && message <= 0xBFFF) {
				appMsg(message);
			}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}