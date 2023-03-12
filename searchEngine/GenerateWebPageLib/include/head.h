#ifndef __head_H__
#define __head_H__
#include<iostream>
#include<fstream>
#include<sstream>

#include<string>
#include<vector>
#include<map>
#include<list>
#include<unordered_map>
#include<set>
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::list;
using std::unordered_map;
using std::set;
using std::pair;


#define ARGS_CHECK(argc,n){\
	if(argc!=n){   \
		fprintf(stderr, "args error :expected %d arguments\n", n);\
		exit(1);\
	}\
}


#define ERROR_CHECK(retVal, val, msg){\
	if(retVal == val){\
		perror(msg); \
		exit(1);    \
	}\
}


#define THREAD_ERROR_CHECK(ret,msg){\
    if(ret!=0){\
    fprintf(stderr,"%s:%s\n",msg,strerror(ret));\
    }\
}




#endif
