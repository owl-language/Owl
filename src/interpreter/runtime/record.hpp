#ifndef record_hpp
#define record_hpp
#include <unordered_map>
#include "memobject.hpp"
using std::string;
using std::unordered_map;

struct Record {
    string name;
    unordered_map<string, int> fieldAddrs;
    Record();
    Record(string name_);
};

#endif