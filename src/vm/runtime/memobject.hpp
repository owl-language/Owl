#ifndef memobject_hpp
#define memobject_hpp
#include <iostream>
using std::string;
using std::to_string;

//Run time Type of data stored in Object
enum RTType {
    INTEGER, REAL, STRING, POINTER, EMPTY
};

inline string rtTypeAsStr[] = { "INTEGER", "REAL", "STRING", "POINTER", "EMPTY"};

//All data, no matter the type is held as an "Object" in memory
//Objects are simple tagged structs, allowing for easy run time type checking
//and all values are passed by reference.
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
    } data;
    struct _attr {
        int size;
        bool isLive;
    } attr;
    Object(const Object& o);
    Object(int val);
    Object(string val);
    Object(float val);
    Object(double val);
    Object();
    Object& operator=(const Object& o);
    string toString();
};


#endif