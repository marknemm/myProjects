#include "RecordHandler.h"
#include <sys/types.h>
#include <sys/stat.h>

/**
* Keeps track of and records mouse directives.
* @param records The previously recorded data that was loaded.
*/
RecordHandler::RecordHandler(RecordsContainer &records)
: _recordsContainer(records),
  _records(records.directives),
  _orderedNames(records.orderedNames),
  _namesOrderedByDate(records.namesOrderedByDate),
  _freedRecordIndices(records.freedRecordIndecees)
{}

RecordHandler::~RecordHandler()
{
	//HMM we may want to do a check on the range of records vs. ordered names
	for(size_t r = 0 ; r < _orderedNames.size() ; r++) {
		delete _records[r], _orderedNames[r], _namesOrderedByDate[r];
	}
}

#define FIX
#ifdef FIX

void RecordHandler::fix()
{
	// Replace action codes
	for(std::vector<Record*>::iterator iter(_records.begin()) ; iter != _records.end() ; iter++) {
		for(int d(0) ; d < (*iter)->numOfSubDirectives() ; d++) {
			if((*iter)->getAction(d) == 2224) {
				(*iter)->setAction(d, K_PRESS);
			}
		}
	}
	save();
}

#endif

/**
* Gets the index of the slot that the next record will be added to.
* @return The index of the next record add slot.
*/
const int RecordHandler::getNextAddIndex() const
{
	if(_freedRecordIndices.size() == 0) {
		return _records.size();
	}
	return _freedRecordIndices.at(0);
}

/**
	 * Changes the title of the record at the specified index.
	 * @param index The index of the record name to be modified.
	 * @param title The new title to be given to the record.
	 */
void RecordHandler::setTitle(const int index, const std::string &title)
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	for(size_t r(_orderedNames.at(_records.at(index)->alphaIndex)->alphaIndex) ; r < _orderedNames.size() ; r++) {
		_orderedNames.erase(_orderedNames.begin() + _orderedNames.at(index)->alphaIndex);
		_orderedNames.at(r)->alphaIndex -= 1;
	}
	_records.at(index)->setTitle(title);
	addRecordToAlphaList(title, _records.at(index));
}

/**
 * Changes the mouse movement destination for the given record and sub-directive of that record.
 * @param recordIndex The index of the record to be modified.
 * @param subIndex The index of the sub-directive of the record to be modified.
 * @param loc The new mouse movement destination.
 */
void RecordHandler::changeMouseDestination(const int recordIndex, const int subIndex, const Point &loc)
{
	RECT res;
	HWND hDesktop(GetDesktopWindow());
	GetClientRect(hDesktop, &res);
	std::stringstream ss;
	if(recordIndex < 0 || recordIndex >= (int)_records.size()) {
		ss << "The parameter recordIndex of value " << recordIndex << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(subIndex < 0 || subIndex >= _records.at(recordIndex)->numOfSubDirectives()) {
		ss << "The parameter subIndex of value " << subIndex << " is outside of the range of the record denoted by recordIndex's directives vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(loc.x < 0 || loc.x > res.right || loc.y < 0 || loc.y > res.bottom) {
		ss << "The parameter loc of value (" << loc.x << ", " << loc.y << ") is outside of the bounds of the screen resolution";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_records.at(recordIndex)->setLocation(subIndex, loc);
}

/**
 * Changes the action delay for the given record and sub-directive of that record.
 * @param recordIndex The index of the record to be modified.
 * @param subIndex The index of the sub-directive to be modified.
 * @param delay The new action delay time (in seconds).
 */
void RecordHandler::changeActionDelay(const int recordIndex, const int subIndex, const double delay)
{
	std::stringstream ss;
	if(recordIndex < 0 || recordIndex >= (int)_records.size()) {
		ss << "The parameter recordIndex of value " << recordIndex << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(subIndex < 0 || subIndex >= _records.at(recordIndex)->numOfSubDirectives()) {
		ss << "The parameter subIndex of value " << subIndex << " is outside of the range of the record denoted by recordIndex's directives vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(delay > 0.5)
		_records.at(recordIndex)->setDelay(subIndex, (int)(delay * 1000));
	else {  _records.at(recordIndex)->setDelay(subIndex, 500);  }
}

/**
* Changes the action for the given record and sub-directive of that record.
* @param recordIndex The index of the record.
* @param subIndex The index of the sub-directive.
* @param actionCode The action code corresponding to the new action.
*/
void RecordHandler::changeAction(const int recordIndex, const int subIndex, const int actionCode)
{
	std::stringstream ss;
	if(recordIndex < 0 || recordIndex >= (int)_records.size()) {
		ss << "The parameter recordIndex of value " << recordIndex << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(subIndex < 0 || subIndex >= _records.at(recordIndex)->numOfSubDirectives()) {
		ss << "The parameter subIndex of value " << subIndex << " is outside of the range of the record denoted by recordIndex's directives vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(!isActionValid(actionCode)) {
		ss << "The parameter actionCode of value " << actionCode << " is not a value of a defined action macro";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_records.at(recordIndex)->setAction(subIndex, actionCode);
}

void RecordHandler::changeText(const int recordIndex, const int subIndex, const std::string text)
{
	std::stringstream ss;
	if(recordIndex < 0 || recordIndex >= (int)_records.size()) {
		ss << "The parameter recordIndex of value " << recordIndex << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(subIndex < 0 || subIndex >= _records.at(recordIndex)->numOfSubDirectives()) {
		ss << "The parameter subIndex of value " << subIndex << " is outside of the range of the record denoted by recordIndex's directives vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_records.at(recordIndex)->setText(subIndex, text);
}

/**
* Gets the title of the record at the specified index.
* @param index The index of the record.
* @return The title of the record.
*/
const std::string RecordHandler::getRecordTitle(const int index) const
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _records.at(index)->getTitle();
}

/**
* Gets the creation date of the record at the specified index.
* @param index The index of the record.
* @return The creation date of the record.
*/
const std::string RecordHandler::getRecordCreationDate(const int index)
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	std::string unformDate = _records.at(index)->getCreationDate();
	if(unformDate.length() > 3) {
		std::stringstream sstream;
		sstream << unformDate.substr(5, 3) << unformDate.substr(8) << "/" << unformDate.substr(0, 4);
		return sstream.str();
	}
	else {  return "---";  }
}

/**
* Gets the unformatted record creation date for the specified index.
* @param index The specified index of the record to get the creation date from.
* @return The unformatted creation date of the specified record.
*/
const std::string RecordHandler::getDateUnformatted(const int index) const
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _records.at(index)->getCreationDate();
}

/**
* Gets the creation time of the record at the specified index.
* @param index The index of the record.
* @return The creation time of the record.
*/
const std::string RecordHandler::getRecordCreationTime(const int index)
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	std::string unformTime = _records.at(index)->getCreationTime();
	if(unformTime.length() != 0) {
		std::stringstream sstream;
		sstream << unformTime.substr(0, 5) << unformTime.substr(8, 3);
		return sstream.str();
	}
	else {  return "";  }
}

/**
* Gets the time including seconds.
* @param index The index of the record to fetch the time from.
* @return The unformatted time (with seconds).
*/
const std::string RecordHandler::getTimeUnformatted(const int index) const
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _records.at(index)->getCreationTime();
}

Record* RecordHandler::duplicateRecord(const int index)
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter index of value " << index << " is outside of the range of the _records vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	Record *newRec = new Record(*(_records.at(index)));
	addRecord(newRec);
	return newRec;
}

/**
* Gets the record at the specified index.
* @param index The index of the record.
* @return The record that was requested.
*/
Record* RecordHandler::getRecord(const int index)
{
	if(index < 0 || index >= (int)_records.size()) {
		std::stringstream ss;
		ss << "Trying to get record at index " << index << " which is outside of the range of record indicees";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _records.at(index);
}

Record* RecordHandler::addDefaultRecord()
{
	RecordStruct recStruct;
	recStruct.actions.push_back(NO_ACT);	// No Action
	recStruct.delays.push_back(2000);
	recStruct.keyCodes.push_back(0);
	Point loc = { 0, 0};
	recStruct.locations.push_back(loc);
	recStruct.locations2.push_back(loc);
	recStruct.offsets.push_back(OffsetStruct());
	recStruct.pressDurations.push_back(0);
	recStruct.texts.push_back("");
	srand(size_t(time(NULL)) + ((_recordHandler->numOfRecords() + _recordHandler->numOfFreedRecords()) * 173));
	int randm = rand() % 899999 + 100000;
	std::stringstream ss;
	ss << "Record " << randm;
	recStruct.title = ss.str();
	Record *record = new Record(recStruct);
	addRecord(record);
	return record;
}


/**
* Adds a new record.
* @param record The new record to be added to the save data.
*/
void RecordHandler::addRecord(Record *record)
{
	try {
		if(record == NULL)
			debugBreak(DEBUG_LOC, "The argument *record is NULL");

		if(_freedRecordIndices.size() != 0) {
			if(_records.at(_freedRecordIndices.begin()->first)->alphaIndex >= (int)_orderedNames.size()) {
				std::stringstream ss;
				ss << "The alpha index queried is greater than the size of the ordered names vector: ";
				ss << _records.at(_freedRecordIndices.begin()->first)->alphaIndex << " >= " << _orderedNames.size();
				debugBreak(DEBUG_LOC, ss.str().c_str());
			}
			_orderedNames.erase(_orderedNames.begin() + _records.at(_freedRecordIndices.begin()->first)->alphaIndex);
			for(size_t r(_records.at(_freedRecordIndices.begin()->first)->alphaIndex) ; r < _orderedNames.size() ; r++)
				_orderedNames.at(r)->alphaIndex -= 1;
			if(_records.at(_freedRecordIndices.begin()->first)->dateIndex >= (int)_namesOrderedByDate.size()) {
				std::stringstream ss;
				ss << "The date index queried is greater than the size of the names ordered by date vector: ";
				ss << _records.at(_freedRecordIndices.begin()->first)->dateIndex << " >= " << _namesOrderedByDate.size();
				debugBreak(DEBUG_LOC, ss.str().c_str());
			}
			_namesOrderedByDate.erase(_namesOrderedByDate.begin() + _records.at(_freedRecordIndices.begin()->first)->dateIndex);
			for(size_t r(_records.at(_freedRecordIndices.begin()->first)->dateIndex) ; r < _namesOrderedByDate.size() ; r++)
				_namesOrderedByDate.at(r)->dateIndex -= 1;
			delete(_records.at(_freedRecordIndices.begin()->first));
			_records.at(_freedRecordIndices.begin()->first) = record->setNumIndex(_freedRecordIndices.begin()->first);
			_freedRecordIndices.erase(_freedRecordIndices.begin());
		}
		else {
			_records.push_back(record->setNumIndex(_records.size()));
			this->numOfRecords();
		}
		addRecordToAlphaList(record->getTitle(), record);
		for(size_t r(_namesOrderedByDate.size() - _freedRecordIndices.size()) ; r < _namesOrderedByDate.size() ; r++)
			_namesOrderedByDate.at(r)->dateIndex += 1;
		_namesOrderedByDate.insert((_namesOrderedByDate.begin() + _namesOrderedByDate.size() - _freedRecordIndices.size())
		  , record->setDateIndex(_namesOrderedByDate.size() - _freedRecordIndices.size()));
	}
	catch(std::exception const &ex) {
		std::stringstream ss;
		ss << "The adding of a record has caused a memory error: " << ex.what();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
}


void RecordHandler::addRecordToAlphaList(const std::string &title, Record *record)
{
	if(record == NULL)
		debugBreak(DEBUG_LOC, "The argument *record is NULL");

	if(_orderedNames.size() == 1 || title.compare(_orderedNames.at(1)->getTitle()) <= 0 || _orderedNames.at(1)->numOfSubDirectives() == 0) {
		for(size_t r(1) ; r < _orderedNames.size() ; r++)
			_orderedNames.at(r)->alphaIndex++;
		_orderedNames.insert((_orderedNames.begin() + 1), record->setAlphaIndex(1));
	}
	else if(title.compare(_orderedNames.at(_orderedNames.size() - 1 - _freedRecordIndices.size())->getTitle()) >= 0) {
		for(size_t r(_orderedNames.size() - _freedRecordIndices.size()) ; r < _orderedNames.size() ; r++)
			_orderedNames.at(r)->alphaIndex++;
		_orderedNames.insert((_orderedNames.begin() + _orderedNames.size() - _freedRecordIndices.size()), record->setAlphaIndex(_orderedNames.size() - _freedRecordIndices.size()));
	}
	else {
		size_t l(1);
		size_t r(_orderedNames.size() - _freedRecordIndices.size());
		size_t m((r + l) / 2);
		while(l < (r - 1)) {
			if(title.compare(_orderedNames.at(m)->getTitle()) < 0)
				r = m;
			else if(title.compare(_orderedNames.at(m)->getTitle()) > 0)
				l = m;
			else {
				l = m;
				r = m;
			}
			m = ((r + l) / 2);
		}
		for(size_t i(r) ; i < _orderedNames.size() ; i++)
			_orderedNames.at(i)->alphaIndex += 1;
		_orderedNames.insert((_orderedNames.begin() + r), record->setAlphaIndex(r));
	}
}


/**
 * Deletes the record at the specified index.
 * @param index The index of the record to be deleted.
 */
void RecordHandler::deleteRecord(const int index)
{
	try {
		if(index < 0 || index >= (int)_records.size()) {
			std::stringstream ss;
			ss << "The index, " << index << ", of the record to be deleted is out of range of record indicees";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		int alphaIndex(_records.at(index)->alphaIndex);
		if(alphaIndex < 0 || alphaIndex >= (int)_orderedNames.size()) {
			std::stringstream ss;
			ss << "The alpha index, " << alphaIndex << ", of the record to be deleted is out of range of ordered names indicees";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		_orderedNames.erase(_orderedNames.begin() + alphaIndex);
		for(size_t r(alphaIndex) ; r < _orderedNames.size() ; r++)
			_orderedNames.at(r)->alphaIndex -= 1;
		_orderedNames.push_back(_records.at(index)->setAlphaIndex(_orderedNames.size()));
		int dateIndex(_records.at(index)->dateIndex);
		if(dateIndex < 0 || dateIndex >= (int)_namesOrderedByDate.size()) {
			std::stringstream ss;
			ss << "The date index, " << dateIndex << ", of the record to be deleted is out of range of names ordered by date indicees";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		_namesOrderedByDate.erase(_namesOrderedByDate.begin() + dateIndex);
		for(size_t r(dateIndex) ; r < _namesOrderedByDate.size() ; r++)
			_namesOrderedByDate.at(r)->dateIndex -= 1;
		_namesOrderedByDate.push_back(_records.at(index)->setDateIndex(_namesOrderedByDate.size()));
		_records.at(index)->clear();
		_freedRecordIndices.insert(std::pair<int, int>(index, index));
	}
	catch(std::exception const &ex) {
		std::stringstream ss;
		ss << "The deleting of a record has caused a memory error: " << ex.what();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
}


/**
* Swaps the records belonging to the two input indices.
* @param index1 The first record's index.
* @param index2 The second record's index.
*/
void RecordHandler::swapRecords(int index1, int index2)
{
	std::stringstream ss;
	if(index1 < 1 || index1 >= (int)_records.size()) { //index 0 is constant for new record creation!!!
		ss << "The argument index1 of value " << index1 << " is out of the range of valid records indicees";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	if(index2 < 1 || index2 >= (int)_records.size()) {
		ss << "The argument index2 of value " << index2 << " is out of the range of valid records indicees";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	Record *temp = _records.at(index2)->setNumIndex(index1);
	_records.at(index2) = _records.at(index1)->setNumIndex(index2);
	_records.at(index1) = temp;
	if(_freedRecordIndices.find(index1) != _freedRecordIndices.end() && _freedRecordIndices.find(index2) == _freedRecordIndices.end()) {
		_freedRecordIndices.erase(index1);
		_freedRecordIndices.insert(std::pair<int, int>(index2, index2));
	}
	else if(_freedRecordIndices.find(index1) == _freedRecordIndices.end() && _freedRecordIndices.find(index2) != _freedRecordIndices.end()) {
		_freedRecordIndices.erase(index2);
		_freedRecordIndices.insert(std::pair<int, int>(index1, index1));
	}
}

/**
* Gets the number of records that have been saved.
* @return The number of saved records.
*/
const int RecordHandler::numOfRecords() const
{
	return _records.size();
}

/**
 * Gets the number of freed record spaces.
 * @return The number of freed record spaces.
 */
const int RecordHandler::numOfFreedRecords() const
{
	return _freedRecordIndices.size();
}

/**
* Gets indices of numerically ordered records from a list in alphabetical order.
* @param index The index of the alphabetically ordered record.
* @return The index of the numerically ordered record.
*/
const int RecordHandler::getIndexFromAlphaOrder(const int index) const
{
	if(index < 0 || index >= (int)_orderedNames.size()) {
		std::stringstream ss;
		ss << "The parameter index of value" << index << " is out of range of the _orderedNames vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _orderedNames.at(index)->numIndex;
}

/**
* Gets indices of numerically ordered records from a list in order by date created (from oldest to youngest).
* @param index The index of the date ordered record.
* @return The index of the numerically ordered record.
*/
const int RecordHandler::getIndexFromDateOrder(const int index) const
{
	if(index < 0 || index >= (int)_namesOrderedByDate.size()) {
		std::stringstream ss;
		ss << "The parameter index of value" << index << " is out of range of the _namesOrderedByDate vector";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _namesOrderedByDate.at(index)->numIndex;
}

/**
* Gets a record index from an ordered list.
* @param search The character base to search for.
*/
const int RecordHandler::getRecordIndexFor(const std::string &search)
{
	int listBuff(_freedRecordIndices.size());
	if(_orderedNames.size() > 0) {
		if(search.length() <= _orderedNames.at(1)->getTitle().length() && search.compare(_orderedNames.at(1)->getTitle().substr(0, search.length())) == 0)
			return _orderedNames.at(1)->numIndex;
		if(search.length() <= _orderedNames.at(_orderedNames.size() - 1 - listBuff)->getTitle().length()
		&& search.compare(_orderedNames.at(_orderedNames.size() - 1 - listBuff)->getTitle().substr(0, search.length())) == 0) {
			return _orderedNames.at(_orderedNames.size() - 1 - listBuff)->numIndex;
		}
		int l(1);
		int r(_orderedNames.size() - listBuff);
		int m((l + r) / 2);
		std::string formStr("");
		while(l != m) {
			if(formStr.compare("") != 0 && search.compare(formStr) < 0)
				r = m;
			else if(formStr.compare("") != 0)
				l = m;
			m = ((l + r) / 2);
			if(_orderedNames.at(m)->getTitle().length() >= search.length()) {
				formStr = _orderedNames.at(m)->getTitle().substr(0, search.length());
				if(formStr.compare(search) == 0)
					return _orderedNames.at(m)->numIndex;
			}
			else {  formStr = _orderedNames.at(m)->getTitle();  }
		}
	}
	return -1;
}

const bool RecordHandler::doesRecordTitleExist(const std::string &search)
{
	int listBuff(_freedRecordIndices.size());
	if(_orderedNames.size() > 0) {
		if(search.length() == _orderedNames.at(1)->getTitle().length() && search.compare(_orderedNames.at(1)->getTitle()) == 0)
			return true;
		if(search.length()==_orderedNames.at(_orderedNames.size()-1-listBuff)->getTitle().length()
		&& search.compare(_orderedNames.at(_orderedNames.size()-1-listBuff)->getTitle())==0)
			return true;
		int l(1);
		int r(_orderedNames.size() - listBuff);
		int m((l + r) / 2);
		std::string formStr = "";
		while(l != m) {
			if(formStr.compare("") != 0 && search.compare(formStr) < 0)
				r = m;
			else if(formStr.compare("") != 0)
				l = m;
			m = ((l + r) / 2);
			if(_orderedNames.at(m)->getTitle().length() == search.length()) {
				formStr = _orderedNames.at(m)->getTitle();
				if(formStr.compare(search) == 0)
					return true;
			}
			else {  formStr = _orderedNames.at(m)->getTitle();  }
		}
	}
	return false;
}

/**
* Specifies whether or not the given record index houses a deleted record slot.
* @param recordIndex The record index to check for a deleted record.
* @return True if it homes a deleted record, False if not.
*/
const bool RecordHandler::isRecordCleared(const int recordIndex) const
{
	if(recordIndex < 0 || recordIndex >= (int)_records.size()) {
		std::stringstream ss;
		ss << "The parameter recordIndex " << recordIndex << " is out of range of the _records vector (Won't cause error here, but might down the line)";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return (_freedRecordIndices.find(recordIndex) != _freedRecordIndices.end());
}

/**
* Dumps all record info, and generates a clean slate.
*/
void RecordHandler::dumpRecords()
{
	_records.clear();
	_orderedNames.clear();
	_namesOrderedByDate.clear();
	_freedRecordIndices.clear();
	RecordStruct recStruct;
	recStruct.index = 0;
	recStruct.title = "Create New Record";
	Record *record0 = new Record(recStruct);
	_records.push_back(record0);
	_orderedNames.push_back(record0);
	_namesOrderedByDate.push_back(record0);
}


void RecordHandler::generateBackup()
{
	SetFileAttributes("DynaHelperBackup.dat", FILE_ATTRIBUTE_NORMAL);
	if(CopyFile("DynaHelper.dat", "DynaHelperBackup.dat", FALSE) == 0) {
		if(GetLastError() != 2) {
			std::stringstream ss;
			ss << "Backup creation on save failed with windows error code: " << GetLastError();
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
	}
	SetFileAttributes("DynaHelperBackup.dat", FILE_ATTRIBUTE_READONLY);
}


void RecordHandler::save()
{
	checkRecordDataStability();
	generateBackup();
	SetFileAttributes("DynaHelper.dat", FILE_ATTRIBUTE_NORMAL);
	std::ofstream ofs("DynaHelper.dat");
	if(!ofs.is_open())
		debugBreak(DEBUG_LOC, "The output stream for the data file could not be established");
	try {
		boost::archive::text_oarchive oa(ofs);
		oa << _recordsContainer;
	}
	catch(boost::archive::archive_exception const &ex) {
		debugBreak(DEBUG_LOC, ex.what());
	}
	ofs.close();
	SetFileAttributes("DynaHelper.dat", FILE_ATTRIBUTE_READONLY);
}


void RecordHandler::checkRecordDataStability()
{
	size_t r;
	std::stringstream ss;
	if(_records.size() != _orderedNames.size() || _records.size() != _namesOrderedByDate.size()) {
		ss << "The records vectors are not the same size " << _records.size() << " == " << _orderedNames.size() << " == " << _namesOrderedByDate.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	for(r = 0 ; r < _records.size() ; r++) {
		if(_records.at(r) == NULL) {
			ss << "_records vector at index " << r << "is NULL";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		if(r != _records.at(r)->numIndex) {
			ss << "_records vector index, " << r << ", does not equal numIndex " << _records.at(r)->numIndex;
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
	}
	for(r = 0 ; r < _orderedNames.size() ; r++) {
		if(_orderedNames.at(r) == NULL) {
			ss << "_orderedNames vector at index " << r << "is NULL";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		if(r != _orderedNames.at(r)->alphaIndex) {
			ss << "_orderedNames vector index, " << r << ", does not equal alphaIndex " << _orderedNames.at(r)->alphaIndex;
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
	}
	for(r = 0 ; r < _namesOrderedByDate.size() ; r++) {
		if(_namesOrderedByDate.at(r) == NULL) {
			ss << "_namesOrderedByDate vector at index " << r << "is NULL";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		if(r != _namesOrderedByDate.at(r)->dateIndex) {
			ss << "_namesOrderedByDate vector index, " << r << ", does not equal dateIndex " << _namesOrderedByDate.at(r)->dateIndex;
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
	}
}


/**
 * Gets all of the records info in the form of a formated string that can be printed for debugging info.
 * ----parameter----
 * std::string &recordInfo: a reference to an empty string
 */
void RecordHandler::getRecordsInfoString(std::string &recordInfo)
{
	std::stringstream ss;
	ss << "========Records Information=======" << std::endl;
	for(size_t r(0) ; r < _records.size() ; r++) {
		ss << "numIndex: " << _records.at(r)->numIndex << std::endl;
		ss << "alphaIndex: " << _records.at(r)->alphaIndex << std::endl;
		ss << "dateIndex: " << _records.at(r)->dateIndex << std::endl;
		ss << "Title: " << _records.at(r)->getTitle() << std::endl;
		ss << "Date: " << _records.at(r)->getCreationDate() << std::endl;
		ss << "Time: " << _records.at(r)->getCreationTime() << std::endl;
		ss << "Num of Directives: " << _records.at(r)->numOfSubDirectives() << std::endl;
		ss << "----------------------" << std::endl;
	}
	ss << std::endl;
	recordInfo.append(ss.str());
}