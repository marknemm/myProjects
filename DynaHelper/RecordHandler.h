#ifndef  RECORD_HANDLER
#define  RECORD_HANDLER
#define  BOOST_LIB_DIAGNOSTIC
#include <vector>
#include <map>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include "Point.h"
#include "Record.h"
#include "RecordsContainer.h"
#include "Main.h"


/**
 * Keeps track of and records mouse directives.
 * @author Mark Nemmer
 */
class RecordHandler {

	RecordsContainer &_recordsContainer;
	std::vector<Record*> &_records;
	std::vector<Record*> &_orderedNames;
	std::vector<Record*> &_namesOrderedByDate;
	std::map<int, int> &_freedRecordIndices;

public:


	RecordHandler();
	~RecordHandler();
	void fix();
	RecordHandler(RecordsContainer &records);
	const int getNextAddIndex() const;
	void setTitle(const int index, const std::string &title);
	void changeMouseDestination(const int recordIndex, const int subIndex, const Point &loc);
	void changeActionDelay(const int recordIndex, const int subIndex, const double delay);
	void changeAction(const int recordIndex, const int subIndex, const int actionCode);
	void changeText(const int recordIndex, const int subIndex, const std::string text);
	const std::string getRecordTitle(const int index) const;
	const std::string getRecordCreationDate(const int index);
	const std::string getDateUnformatted(const int index) const;
	const std::string getRecordCreationTime(const int index);
	const std::string getTimeUnformatted(const int index) const;
	Record* duplicateRecord(const int index);
	Record* getRecord(const int index);
	Record* addDefaultRecord();
	void addRecord(Record *record);
	void addRecordToAlphaList(const std::string &title, Record *record);
	void deleteRecord(const int index);
	void swapRecords(const int index1, const int index2);
	const int numOfRecords() const;
	const int numOfFreedRecords() const;
	const int getIndexFromAlphaOrder(const int index) const;
	const int getIndexFromDateOrder(const int index) const;
	const int getRecordIndexFor(const std::string &search);
	const bool doesRecordTitleExist(const std::string &search);
	const bool isRecordCleared(const int recordIndex) const;
	void dumpRecords();
	void save();
	void checkRecordDataStability();
	void getRecordsInfoString(std::string &recordInfo);


private:


	void generateBackup();
};


extern RecordHandler *_recordHandler;


#endif