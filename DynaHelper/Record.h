#ifndef  RECORD_H
#define  RECORD_H
#include <vector>
#include <math.h>
#include <ctime>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include "ActionMacros.h"
#include "Point.h"
#include "OffsetStruct.h"
#include "RecordStruct.h"
#include "Main.h"


class Record {

	std::vector<int> _delays;
	std::vector<int> _pressDurations;
	std::vector<Point> _locations;
	std::vector<Point> _locations2;
	std::vector<int> _actions;
	std::vector<int> _keyCodes;
	std::vector<OffsetStruct> _offsets;
	std::vector<std::string> _texts;
	std::string _title;
	std::string _date;
	std::string _time;


public:
	
	
	bool isTemplate;
	int numIndex;
	int alphaIndex;
	int dateIndex;


private:
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & _delays;
		ar & _pressDurations;
		ar & _locations;
		ar & _locations2;
		ar & _actions;
		ar & _keyCodes;
		ar & _offsets;
		ar & _texts;
		ar & _title;
		ar & _date;
		ar & _time;
		ar & alphaIndex;
		ar & dateIndex;
		ar & numIndex;
	}


public:
	
	
	Record();
	Record(Record &record);
	Record(RecordStruct &recordStruct);
	void setTimeStamp();
	const Point getLocation(const int index) const;
	const Point getLocation2(const int index) const;
	const int getDelay(const int index) const;
	const int getPressDuration(const int index) const;
	const int getAction(const int index) const;
	const int getKeyCode(const int index) const;
	const int getOffsetAction(const int index) const;
	const int getOffsetDeltaX(const int index) const;
	const int getOffsetDeltaY(const int index) const;
	std::string Record::getText(const int index) const;
	std::string getTitle() const;
	std::string getCreationDate() const;
	std::string getCreationTime() const;
	Record* setNumIndex(const int index);
	Record* setAlphaIndex(const int index);
	Record* setDateIndex(const int index);
	void addDefaultDirective(const int index);
	void deleteDirective(const int index);
	void setTitle(const std::string title);
	void setText(const int index, const std::string text);
	void setLocation(const int index, const Point location);
	void setLocation2(const int index, const Point location2);
	void setDelay(const int index, const int delay);
	void setPressDuration(const int index, const int duration);
	void setAction(const int index, const int actionCode);
	void setKeyCode(const int index, const int keyCode);
	void setOffsetAction(const int index, const int action);
	void setOffsetDeltaX(const int index, const int deltaX);
	void setOffsetDeltaY(const int index, const int deltaY);
	int numOfSubDirectives() const;
	void clear();
};


#endif