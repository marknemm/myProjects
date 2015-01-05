#ifndef  OFFSET_STRUCT_H
#define  OFFSET_STRUCT_H
#include <boost/archive/basic_binary_iarchive.hpp>


struct OffsetStruct {
	friend class boost::serialization::access;
	int deltaX, deltaY;
	int action;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & deltaY;
		ar & deltaX;
		ar & action;
	}
};


#endif