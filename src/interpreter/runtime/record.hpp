#ifndef record_hpp
#define record_hpp
#include <map>
#include <iostream>
using std::string;
using std::map;
using std::cout;
using std::endl;

struct Record {
    string name;
    map<string, int> fieldAddrs;
    Record();
    Record(string name_);
    Record(const Record& rec);
    ~Record() {
        cout<<name<<" out of scope and signing off."<<endl;
    }
    Record& operator=(const Record& o) {
        name = o.name;
        for (auto m : o.fieldAddrs) {
            fieldAddrs[m.first] = m.second;
        }
        return *this;
    }
};

#endif