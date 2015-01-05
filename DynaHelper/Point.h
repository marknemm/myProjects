#ifndef  MYPOINT_H
#define  MYPOINT_H
#include <boost/archive/basic_binary_iarchive.hpp>


struct Point {
	friend class boost::serialization::access;
	int x,y;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & x;
		ar & y;
	}
};


#endif