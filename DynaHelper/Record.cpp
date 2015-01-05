#include "Record.h"
#include "RecordHandler.h"


Record::Record(Record &record)
: _title(record._title),
  isTemplate(record.isTemplate),
  _delays(record._delays),
  _pressDurations(record._pressDurations),
  _locations(record._locations),
  _locations2(record._locations2),
  _actions(record._actions),
  _keyCodes(record._keyCodes),
  _offsets(record._offsets),
  _texts(record._texts)
{
	setTimeStamp();
}


/**
-----Description-----
A record that contains a series of events and data concerning how to carry out those events.
-----Parameter(s)----
RecordStruct &recordStruct: A reference to a record structure that contains most of the major data fields of the Record class (used for its initialization).
*/
Record::Record(RecordStruct &recordStruct)
: _title(recordStruct.title),
  numIndex(recordStruct.index),
  alphaIndex(0),
  dateIndex(0),
  isTemplate(false),
  _delays(recordStruct.delays),
  _pressDurations(recordStruct.pressDurations),
  _locations(recordStruct.locations),
  _locations2(recordStruct.locations2),
  _actions(recordStruct.actions),
  _keyCodes(recordStruct.keyCodes),
  _offsets(recordStruct.offsets),
  _texts(recordStruct.texts)
{
	setTimeStamp();
}


void Record::setTimeStamp()
{
	const time_t rawtime = std::time(0);
	struct tm timeinfo;
	std::vector<char> dateBuffer(80);
	localtime_s(&timeinfo, &rawtime);
	if(strftime(dateBuffer.data(), dateBuffer.size(), "%Y/%m/%d", &timeinfo) == 0)
		debugBreak(DEBUG_LOC, "Date buffer is not large enough");
	_date = dateBuffer.data();
	std::vector<char> timeBuffer(80);
	if(strftime(timeBuffer.data(), timeBuffer.size(), "%I:%M:%S %p", &timeinfo) == 0)
		debugBreak(DEBUG_LOC, "Time buffer is not large enough");
	_time = timeBuffer.data();
}


Record::Record()
: _title(""),
  _date(""),
  _time(""),
  numIndex(0),
  alphaIndex(0),
  dateIndex(0),
  _delays(),
  _pressDurations(),
  _locations(),
  _locations2(),
  _actions(),
  _keyCodes(),
  _offsets(),
  _texts(),
  isTemplate(false)
{}

void Record::addDefaultDirective(const int index)
{
	if(index < 0 || index > _actions.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than the size of the _acitons vector with size " << _actions.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_actions.insert((_actions.begin() + index), DEFAULT_ACT);
	_delays.insert((_delays.begin() + index), 2000);
	Point point = { 0, 0};
	_locations.insert((_locations.begin() + index), point);
	_locations2.insert((_locations2.begin() + index), point);
	_pressDurations.insert((_pressDurations.begin() + index), 0);
	_keyCodes.insert((_keyCodes.begin() + index), 0);
	_texts.insert((_texts.begin() + index), "");
	_offsets.insert((_offsets.begin() + index), OffsetStruct());
}


void Record::deleteDirective(const int index)
{
	if(index < 0 || index >= _actions.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _acitons vector with size " << _actions.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_actions.erase(_actions.begin() + index);
	_delays.erase(_delays.begin() + index);
	_locations.erase(_locations.begin() + index);
	_locations2.erase(_locations2.begin() + index);
	_pressDurations.erase(_pressDurations.begin() + index);
	_keyCodes.erase(_keyCodes.begin() + index);
	_texts.erase(_texts.begin() + index);
	_offsets.erase(_offsets.begin() + index);
}


/**
* Gets the location to move the mouse to.
* @return The (screen) location to move the mouse to.
*/
const Point Record::getLocation(const int index) const
{
	if(index < 0 || index >= _locations.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _locations vector with size " << _locations.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _locations.at(index);
}


/**
-----Description------
Gets the end (second) point location for a click and drag event.
-----Parameter(s)-----
const int index: The record index that the drag endpoint will be retrieved from.
-----Return-----------
const Point: The end (second) point location for a click and drag event ({-1,-1} if there is no click and drag event).
*/
const Point Record::getLocation2(const int index) const
{
	if(index < 0 || index >= _locations2.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _locations2 vector with size " << _locations2.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _locations2.at(index);
}


/**
* Gets the delay at the specified index.
* @return The delay before the action occurs in milliseconds.
*/
const int Record::getDelay(const int index) const
{
	if(index < 0 || index >= _delays.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _delays vector with size " << _delays.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _delays.at(index);
}


/**
-----Description------
Gets the press duration at the specified index.
-----Parameter(s)-----
const int index: The record index that the press duration will be retrieved from.
-----Return-----------
const int: The duration of the press event in milliseconds (0 if there is no press event associated with the index)
*/
const int Record::getPressDuration(const int index) const
{
	if(index < 0 || index >= _pressDurations.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _pressDurations vector with size " << _pressDurations.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _pressDurations.at(index);
}


/**
* Gets the action at the specified index.
* @return The letter code for the mouse action that should occur.
*/
const int Record::getAction(const int index) const
{
	if(index < 0 || index >= _actions.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _actions vector with size " << _actions.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _actions.at(index);
}


/**
-----Description------
Gets the key code associated with the key event of a specified index of the requested record.
-----Parameter(s)-----
const int index: The record index that the key code will be retrieved from.
-----Return-----------
const int: The key code associated with the key event (if there is no key event for the record's index, -1 is returned).
*/
const int Record::getKeyCode(const int index) const
{
	if(index < 0 || index >= _keyCodes.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _keyCodes vector with size " << _keyCodes.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _keyCodes.at(index);
}


/**
-----Description------
Gets the action character associated with the offset event of a specified index of the requested record.
-----Parameter(s)-----
const int index: The record index that the offset event's action character will be retrieved from.
-----Return-----------
const char: The character representing the action associated with the offset event (if there is no offset event, the ' ' character will be returned).
*/
const int Record::getOffsetAction(const int index) const
{
	if(index < 0 || index >= _offsets.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _offsets vector with size " << _offsets.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _offsets.at(index).action;
}


/**
-----Description------
Gets the offset magnitude (size of the offset mouse movement) associated with the offset event of a specified index of the requested record.
-----Parameter(s)-----
const int index: The record index that the offset event's magnitude will be retrieved from.
-----Return-----------
const int: The magnitude for the mouse moevement associated with the offset event (if there is no offset event, -1 will be entered).
*/
const int Record::getOffsetDeltaX(const int index) const
{
	if(index < 0 || index >= _offsets.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _offsets vector with size " << _offsets.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _offsets.at(index).deltaX;
}


/**
-----Description------
Gets the delta-Y value (size of the offset mouse movement in the y-axis direction) associated with the offset event of a specified index of the requested record.
-----Parameter(s)-----
const int index: The record index that the offset event's deltaY value will be retrieved from.
-----Return-----------
const int: The deltaY value for the mouse moevement associated with the offset event (if there is no offset event, -1 will be returned).
*/
const int Record::getOffsetDeltaY(const int index) const
{
	if(index < 0 || index >= _offsets.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _offsets vector with size " << _offsets.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _offsets.at(index).deltaY;
}


/**
-----Description------
Gets the output text of the fetched record's requested index.
-----Parameter(s)-----
const int index: The record index to get the text from.
-----Return-----------
const string: The text associated with the fetched record index (returns an empty string if there is no such text).
*/
std::string Record::getText(const int index) const
{
	if(index < 0 || index >= (int)_texts.size()) {
		std::stringstream ss;
		ss << "The argument index of value " << index << " is neagative or greater than or equal to the size of the _texts vector with size " << _texts.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _texts.at(index);
}


/**
* Gets the Directive's user generated title.
* @return The title of the Directive.
*/
std::string Record::getTitle() const
{
	return _title;
}


/**
* Gets the date of record creation.
* @return The date of record creation.
*/
std::string Record::getCreationDate() const
{
	return _date;
}


/**
* Gets the time of record creation.
* @return The time of record creation.
*/
std::string Record::getCreationTime() const
{
	return _time;
}


/**
* Sets the numerical order list index. (for swapping)
* @param index The index to be set.
* @return The record.
*/
Record* Record::setNumIndex(const int index)
{
	if(index < 1 || index > _recordHandler->numOfRecords()) { //Can be 1 above range on add
		std::stringstream ss;
		ss << "Error: Set Number Index at " << index << " is out of bounds!!!" << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	numIndex = index;
	return this;
}


/**
* Sets the alphabetical order list index.
* @param index The index to be set.
* @return The record.
*/
Record* Record::setAlphaIndex(const int index)
{
	if(index < 1 || index > _recordHandler->numOfRecords()) {
		std::stringstream ss;
		ss << "Error: Set Number Index at " << index << " is out of bounds!!!" << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	alphaIndex = index;
	return this;
}

/**
* Sets the date order list index.
* @param index The index to be set.
* @return The record.
*/
Record* Record::setDateIndex(const int index)
{
	if(index < 1 || index > _recordHandler->numOfRecords()) {
		std::stringstream ss;
		ss << "Error: Set Number Index at " << index << " is out of bounds!!!" << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	dateIndex = index;
	return this;
}

/**
* Sets the Directive's title.
* @param title The new title.
*/
void Record::setTitle(const std::string title)
{
	_title = title;
}

void Record::setText(const int index, const std::string text)
{
	if(index < 0 || index >= (int)_texts.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _texts vector size of " << _texts.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_texts.at(index) = text;
}

/**
* Sets the Directive's location command at the given index.
* @param index The index of the location command to be set.
* @param location The new location.
*/
void Record::setLocation(const int index, const Point location)
{
	if(index < 0 || index >= (int)_locations.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _locations vector size of " << _locations.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_locations.at(index) = location;
}

void Record::setLocation2(const int index, const Point location2)
{
	if(index < 0 || index >= (int)_locations2.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _locations2 vector size of " << _locations2.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_locations2.at(index) = location2;
}


/**
* Sets the Directive's delay at the given index.
* @param index The index to be set.
* @param delay The new delay.
*/
void Record::setDelay(const int index, const int delay)
{
	if(index < 0 || index >= (int)_delays.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _delays vector size of " << _delays.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_delays.at(index) = delay;
}


void Record::setPressDuration(const int index, const int duration)
{
	if(index < 0 || index >= (int)_pressDurations.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _pressDurations vector size of " << _pressDurations.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_pressDurations.at(index) = duration;
}


/**
* Sets the Directive's action at the given index.
* @param index The index to be set.
* @param actionCode The new action (Character Code).
*/
void Record::setAction(const int index, const int actionCode)
{
	if(index < 0 || index >= (int)_actions.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _actions vector size of " << _actions.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_actions.at(index) = actionCode;
}


void Record::setKeyCode(const int index, const int keyCode)
{
	if(index < 0 || index >= (int)_keyCodes.size()) {
		std::stringstream ss;
		ss << "The index " << index << " is out of range of the _keyCodes vector size of " << _keyCodes.size() << std::endl;
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_keyCodes.at(index) = keyCode;
}


void Record::setOffsetAction(const int index, const int action)
{
	_offsets.at(index).action = action;
}


void Record::setOffsetDeltaX(const int index, const int deltaX)
{
	_offsets.at(index).deltaX = deltaX;
}


void Record::setOffsetDeltaY(const int index, const int deltaY)
{
	_offsets.at(index).deltaY = deltaY;
}


/**
* Gets the number of sub-directives present for this directive.
* @return The number of sub-directives.
*/
int Record::numOfSubDirectives() const
{
	return _actions.size();
}


/**
* Clears all field values.
*/
void Record::clear()
{
	_title = "---";
	_delays.clear();
	_actions.clear();
	_locations.clear();
	_keyCodes.clear();
	_locations2.clear();
	_offsets.clear();
	_pressDurations.clear();
	_texts.clear();
	_date = "---";
	_time = "";
}