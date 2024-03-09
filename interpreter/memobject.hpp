#ifndef memobject_hpp
#define memobject_hpp
#include <iostream>
using namespace std;

//Run time Type of data stored in Object
enum RTType {
    INTEGER, REAL, STRING, POINTER, EMPTY
};

string rtTypeAsStr[] = { "INTEGER", "REAL", "STRING", "POINTER", "EMPTY"};

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

Object::Object() {
    data._value = "0";
    attr.size = 1;
    attr.isLive = false;
    type = EMPTY;
}

Object::Object(const Object& o) {
    type = o.type;
    data._value = o.data._value;
    attr.size = o.attr.size;
    attr.isLive = o.attr.isLive;
}

Object::Object(int val) {
    data._value = to_string(val);
    attr.size = 1;
    attr.isLive = true;
    type = INTEGER;
}

Object::Object(string val) {
    data._value = val;
    attr.size = val.size();
    attr.isLive = true;
    type = STRING;
}

Object::Object(float val) {
    data._value = to_string(val);
    attr.isLive = true;
    attr.size = 1;
    type = REAL;
}

Object::Object(double val) {
    data._value = to_string(val);
    attr.isLive = true;
    attr.size = 1;
    type = REAL;
}

Object& Object::operator=(const Object& o) {
    type = o.type;
    data._value = o.data._value;
    attr.size = o.attr.size;
    attr.isLive = o.attr.isLive;
    return *this;
}

string Object::toString() {
    return data.stringValue();
}

#endif