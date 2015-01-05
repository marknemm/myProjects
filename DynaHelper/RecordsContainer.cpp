#include"RecordsContainer.h"


RecordsContainer::RecordsContainer()
: directives(std::vector<Record*>()),
  orderedNames(std::vector<Record*>()),
  namesOrderedByDate(std::vector<Record*>()),
  freedRecordIndecees(std::map<int,int>())
{
	RecordStruct recStruct0;
	recStruct0.index = 0;
	recStruct0.title = "Create New Record";
	Record *record0 = new Record(recStruct0);
	directives.push_back(record0);
	orderedNames.push_back(record0);
	namesOrderedByDate.push_back(record0);
}