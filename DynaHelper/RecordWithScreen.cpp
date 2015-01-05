#include "RecordWithScreen.h"


/**
-----Description-----
DEFAULT_ACT constructor for RecordScreen class (to be used at the start-up of DynaHelper program).
NOTE: the void return founction "initialize(_globHINST hInst, RecordHandler *recordHandler)" should be used to initialize
the RecordScreen class for use later on after the parameters become available!
*/
RecordWithScreen::RecordWithScreen()
: _index(0),
  _recordsAdded(0),
  _recordScreen(NULL)
{
	activate(0);
}


bool RecordWithScreen::enableActionMenu(ActionMenu **am, int &action)
{
	Sleep(ACTION_SLEEP_AMT);
	if(!isActionValid(action)) {
		std::stringstream ss;
		ss << "The argument action of value " << action << " is not a defined action macro";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(_index == _recordStruct.actions.size())
		*am = new ActionMenu(DEFAULT_ACT, (_index > 0));
	else
		*am = new ActionMenu(_recordStruct.actions.at(_index), (_index > 0));
	action = (*am)->waitForInput();
	if((*am)->wasBackEntered()) {
		delete(*am);
		continueRecording(_recordStruct.keyCodes.at(--_index), 3);
		return false;
	}
	if(!(*am)->isActionSelected()) {
		delete(*am);
		reset();
		return false;
	}
	return true;
}


void RecordWithScreen::setRecordStructAction(int action, int stage)
{
	if(!isActionValid(action)) {
		std::stringstream ss;
		ss << "The argument action of value " << action << " is not a defined action macro";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(_index == _recordStruct.actions.size())
		_recordStruct.actions.push_back(action);
	else {  _recordStruct.actions.at(_index) = action;  }
}


int RecordWithScreen::getRecordScreenResults()
{
	if(_index == _recordStruct.locations.size())
		_recordStruct.locations.push_back(_recordScreen->locationEntered());
	else
		_recordStruct.locations.at(_index) = _recordScreen->locationEntered();
	if(_index == _recordStruct.locations2.size())
		_recordStruct.locations2.push_back(_recordScreen->location2Entered());
	else
		_recordStruct.locations2.at(_index) = _recordScreen->location2Entered();
	return _recordScreen->keyEntered();
}


void RecordWithScreen::activate(int stage)
{
	int keyEntered(0);
	ActionMenu *am = NULL;
	int action(DEFAULT_ACT);
	if(stage <= 1) {
		if(enableActionMenu(&am, action) == false)
			return;
		setRecordStructAction(action, stage);
	}
	if((stage == 2) || (am->isActionSelected())) {
		if(_recordStruct.actions.at(_index) == TEXT_INP) {
			continueRecording(-1, 0);
			return;
		}
		else {
			if(_recordScreen != NULL) {
				delete(_recordScreen);
				_recordScreen = NULL;
			}
			Sleep(ACTION_SLEEP_AMT);
			_recordScreen = new RecordScreen(true);
			_recordScreen->waitForInput(_recordStruct.actions.at(_index));
			if(_recordScreen->wasBackEntered()) {
				activate(0);
				return;
			}
			else {  keyEntered = getRecordScreenResults();  }
		}
	}
	else if(am->wasBackEntered()) {
		_index--;
		continueRecording(0, 3);
	}
	else {  reset();  }
	if(am != NULL) {  delete(am);  }
	continueRecording(keyEntered, 0);
}


bool RecordWithScreen::enableTextInputMenu(std::string &text)
{
	Sleep(ACTION_SLEEP_AMT);
	TextInput *textIn = NULL;
	if(_index == _recordStruct.texts.size()) {
		textIn = new TextInput("");
	}
	else {
		textIn = new TextInput(_recordStruct.texts.at(_index));
	}
	text = textIn->waitForInput();
	if(!textIn->wasInputEntered()) {
		delete(textIn);
		reset();
		return false;
	}
	else if(textIn->wasBackEntered()) {
		delete(textIn);
		activate(1);
		return false;
	}
	delete(textIn);
	return true;
}


bool RecordWithScreen::enableKeyInputVerificationMenu(int keyCode)
{
	Sleep(ACTION_SLEEP_AMT);
	KeyInputVerificationMenu keyInVer(keyCode);
	BOOL decision(keyInVer.waitForDecision());
	if(!keyInVer.wasInputEntered()) {
		reset();
		return false;
	}
	else if(!decision || keyInVer.wasBackEntered()) {
		activate(2);
		return false;
	}
	return true;
}


bool RecordWithScreen::enableNumScrollsMenu(int &duration)
{
	Sleep(ACTION_SLEEP_AMT);
	NumOfScrollsMenu *numScrollMen = NULL;
	if(_index == _recordStruct.pressDurations.size()) {
		numScrollMen = new NumOfScrollsMenu(0);
	}
	else {
		numScrollMen = new NumOfScrollsMenu(_recordStruct.pressDurations.at(_index));
	}
	duration = numScrollMen->waitForInput();
	if(!numScrollMen->wasInputEntered()) {
		delete(numScrollMen);
		reset();
		return false;
	}
	else if(numScrollMen->wasBackEntered()) {
		delete(numScrollMen);
		activate(2);
		return false;
	}
	delete(numScrollMen);
	return true;
}


bool RecordWithScreen::enableOffsetActionMenu(OffsetStruct &offset, int &duration)
{
	Sleep(ACTION_SLEEP_AMT);
	ActionOffsetMenu *offActMen = NULL;
	if(_index == _recordStruct.offsets.size()) {
		offActMen = new ActionOffsetMenu(DEFAULT_ACT);
	}
	else {
		offActMen = new ActionOffsetMenu(_recordStruct.offsets.at(_index).action);
	}
	int offact(offActMen->waitForInput());
	if(offActMen->wasBackEntered()) {
		delete(offActMen);
		activate(2);
		return false;
	}
	else if(!offActMen->wasInputEntered()) {
		delete(offActMen);
		reset();
		return false;
	}
	if(offact == TEXT_INP) {
		offact = NO_ACT;
	}
	offset.action = offact; 
	offset.deltaX = (_recordStruct.locations2.at(_index).x - _recordStruct.locations.at(_index).x); 
	offset.deltaY = (_recordStruct.locations2.at(_index).y - _recordStruct.locations.at(_index).y);
	delete(offActMen);
	if(stage1Action(0, 0, offact, duration, offset, std::string("")) == false) {
		return false;
	}
	return true;
}


bool RecordWithScreen::enablePressDurationMenu(int &duration, int stage)
{
	Sleep(ACTION_SLEEP_AMT);
	PressDurationMenu *pressDurMen = NULL;
	if((_index == _recordStruct.pressDurations.size()) && (stage == 0)) {
		pressDurMen = new PressDurationMenu(0.0);
	}
	else {
		pressDurMen = new PressDurationMenu(double(_recordStruct.pressDurations.at(_index)) / 1000.0);
	}
	duration = pressDurMen->waitForDuration();
	if(!pressDurMen->wasDurationEntered()) {
		delete(pressDurMen);
		reset();
		return false;
	}
	else if(pressDurMen->wasBackEntered()) {
		delete(pressDurMen);
		activate(2);
		return false;
	}
	if(duration < 10) {
		duration = 10;
	}
	delete(pressDurMen);
	return true;
}


void RecordWithScreen::fillData(int stage, std::string &text, OffsetStruct &offset, int &duration, int &keyCode)
{
	if(stage > 1) {
		text = _recordStruct.texts.at(_index);
		offset = _recordStruct.offsets.at(_index);
		duration = _recordStruct.pressDurations.at(_index);
	}
	if(_index == _recordStruct.pressDurations.size()) {
		_recordStruct.pressDurations.push_back(duration);
		_recordStruct.keyCodes.push_back(keyCode);
		_recordStruct.texts.push_back(text);
		_recordStruct.offsets.push_back(offset);
		if(_recordStruct.locations.size() != _recordStruct.actions.size()) {
			Point loc = { -1 , -1 };
			_recordStruct.locations.push_back(loc);
			_recordStruct.locations2.push_back(loc);
		}
		else if(_recordStruct.locations2.size() != _recordStruct.actions.size()) {
			Point loc2 = { -1 , -1 };
			_recordStruct.locations2.push_back(loc2);
		}
	}
	else {
		_recordStruct.pressDurations.at(_index) = duration;
		_recordStruct.keyCodes.at(_index) = keyCode;
		_recordStruct.texts.at(_index) = text;
		_recordStruct.offsets.at(_index) = offset;
	}
}


bool RecordWithScreen::enableActionDelayMenu(int keyCode)
{
	Sleep(ACTION_SLEEP_AMT);
	ActionDelayMenu *adm = NULL;
	bool lowerDelayOk(false);
	if(_index != _recordStruct.delays.size()) {
		adm = new ActionDelayMenu(lowerDelayOk, (double(_recordStruct.delays.at(_index)) / 1000.0));
	}
	else if(_index > 0) {
		lowerDelayOk = ((_recordStruct.actions.at(_index - 1) == K_PRESS) && (_recordStruct.actions.at(_index) & KEY));
		adm = new ActionDelayMenu(lowerDelayOk, 0.0);
	}
	else {
		adm = new ActionDelayMenu(lowerDelayOk, 0.0);
	}
	int d(adm->waitForDelay(lowerDelayOk));
	if(!adm->wasDelayEntered()) {
		delete(adm);
		reset();
		return false;
	}
	else if(adm->wasBackEntered()) {
		delete(adm);
		if(_recordStruct.actions.at(_index) & EXTRA_STAGE) {
			continueRecording(keyCode, 1);
		}
		else {  activate(2);  }
		return false;
	}
	if(_index == _recordStruct.delays.size()) {
		_recordStruct.delays.push_back(d);
	}
	else {
		_recordStruct.delays.at(_index) = d;
	}
	delete(adm);
	return true;
}


bool RecordWithScreen::enableFinishedMenu(bool &finished, int keyCode)
{
	Sleep(ACTION_SLEEP_AMT);
	FinishedMenu fm;
	finished = !(fm.waitForDecision());
	if(!fm.wasInputEntered()) {
		reset();
		return false;
	}
	else if(fm.wasBackEntered()) {
		continueRecording(keyCode, 2);
		return false;
	}
	return true;
}


bool RecordWithScreen::enableTitleMenu(int keyCode)
{
	Sleep(ACTION_SLEEP_AMT);
	TitleMenu *tm = NULL;
	if(_recordStruct.title == "") {
		tm = new TitleMenu(((_recordsAdded++) * 173), "");
	}
	else {
		tm = new TitleMenu(0, _recordStruct.title);
	}
	_recordStruct.title = tm->waitForTitle();
	if(!tm->wasTitleEntered()) {
		delete(tm);
		reset();
		return false;
	}
	else if(tm->wasBackEntered()) {
		delete(tm);
		continueRecording(keyCode, 3);
		return false;
	}
	delete(tm);
	return true;
}


void RecordWithScreen::save()
{
	Record *record = new Record(_recordStruct);
	_recordHandler->addRecord(record);
	_recordHandler->save();
	reset();
}


bool RecordWithScreen::stage1Action(int keyCode, int stage, int action, int &duration, OffsetStruct &offset, std::string &text)
{
	if(action & EXTRA_STAGE) {
		if(action == TEXT_INP) {
			return enableTextInputMenu(text);
		}
		else if((action == K_TYPE) && (stage != 1)) {
			return enableKeyInputVerificationMenu(keyCode);
		}
		else if(action & SCROLL) {
			return enableNumScrollsMenu(duration);
		}
		else if(action & RELATIV) {
			return enableOffsetActionMenu(offset, duration);
		}
		else if(action & DURATION) {
			if((action == K_PRESS) && (enableKeyInputVerificationMenu(keyCode) == false)) {
				return false;
			}
			return enablePressDurationMenu(duration, stage);
		}
	}
	return true;
}


/**
-----Description-----
Initiates and executes the process of recording after a click has been registered on the recording screen.
-----Parameters-----
int keyCode: The key code of an event assocaite with the keyboard (-1 if mouse event).
int stage: The stage that the function will allow access to (0 being all stages).
*/
void RecordWithScreen::continueRecording(int keyCode, int stage)
{
	int duration(1500);
	OffsetStruct offset = OffsetStruct();
	std::string text("");

	if(stage <= 1) {
		if(stage1Action(keyCode, stage, _recordStruct.actions.at(_index), duration, offset, text) == false) {  return;  }
	}

	fillData(stage, text, offset, duration, keyCode);

	if(stage <= 2) {
		if(enableActionDelayMenu(keyCode) == false) {  return;  }
	}
	
	bool finished(false);
	if(enableFinishedMenu(finished, keyCode) == false) {  return;  }

	if(finished) {
		if(enableTitleMenu(keyCode) == false) {  return;  }
		save();
	}
	else {
		_index++;
		activate(0);
	}
}


/**
-----Description-----
Resets all of the data elements for adding a brand new Record.
*/
void RecordWithScreen::reset()
{
	_recordStruct.index = 0;
	_recordStruct.title = "";
	_recordStruct.locations.clear();
	_recordStruct.delays.clear();
	_recordStruct.actions.clear();
	_recordStruct.texts.clear();
	_recordStruct.locations2.clear();
	_recordStruct.keyCodes.clear();
	_recordStruct.offsets.clear();
	_recordStruct.pressDurations.clear();
	if(_recordScreen != NULL) {
		_recordScreen->sendMessage(WM_CLOSE, NULL, NULL);
		delete(_recordScreen);
	}
	refresh();
}