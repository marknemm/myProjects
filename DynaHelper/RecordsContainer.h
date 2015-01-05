#ifndef  RECORDS_CONTAINER_H
#define  RECORDS_CONTAINER_H
#include <vector>
#include <map>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>
#include "RecordStruct.h"
#include "Record.h"


class RecordsContainer {

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		ar & directives;
		ar & orderedNames;
		ar & namesOrderedByDate;
		ar & freedRecordIndecees;
	}


public:


	std::vector<Record*> directives;
	std::vector<Record*> orderedNames;
	std::vector<Record*> namesOrderedByDate;
	std::map<int,int> freedRecordIndecees;
	RecordsContainer();
};


#endif