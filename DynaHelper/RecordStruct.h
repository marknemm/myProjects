#ifndef  RECORD_STRUCT_H
#define  RECORD_STRUCT_H
#include <vector>
#include <string>
#include "Point.h"
#include "OffsetStruct.h"


struct RecordStruct {
	int index;
	std::vector<int> delays;
	std::vector<int> pressDurations;
	std::vector<Point> locations;
	std::vector<Point> locations2;
	std::vector<int> actions;
	std::vector<int> keyCodes;
	std::vector<OffsetStruct> offsets;
	std::vector<std::string> texts;
	std::string title;
};


#endif