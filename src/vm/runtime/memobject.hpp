#ifndef memobject_hpp
#define memobject_hpp
#include <iostream>
#include <list>
#include <unordered_map>
using std::string;
using std::to_string;
using std::unordered_map;

//Run time Type of data stored in Object
enum RTType {
    INTEGER, REAL, STRING, POINTER, HASH, EMPTY
};

inline string rtTypeAsStr[] = { "INTEGER", "REAL", "STRING", "POINTER", "HASH", "EMPTY"};

struct _attr {
    int size;
    bool isLive;
    _attr() : size(1), isLive(false) { }
};

//All data, no matter the type is held as an "Object" in memory
//Objects are simple tagged structs, allowing for easy run time type checking
struct Object {
    RTType type;
    struct _data {
        string _value;
        int intValue() {
            return atoi(_value.c_str());
        }
        float realValue() {
            return stof(_value);
        }
        string stringValue() {
            return _value;
        }
        _data() {
            _value = "none";
        }
    } data;
    _attr attr;
    Object(const Object& o);
    Object(int val);
    Object(string val);
    Object(float val);
    Object(double val);
    Object();
    Object& operator=(const Object& o);
    string toString();
};

inline const int TABLE_SIZE = 211;


#endif