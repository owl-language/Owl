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
        int intValue;
        double realValue;
        string stringValue;
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
    data.stringValue = "0";
    data.realValue = 0.0f;
    data.intValue = 0;
    attr.size = 1;
    attr.isLive = false;
    type = EMPTY;
}

Object::Object(const Object& o) {
    type = o.type;
    data.intValue = o.data.intValue;
    data.realValue = o.data.realValue;
    data.stringValue = o.data.stringValue;
    attr.size = o.attr.size;
    attr.isLive = o.attr.isLive;
}

Object::Object(int val) {
    data.intValue = val;
    data.realValue = (float) val;
    data.stringValue = to_string(val);
    attr.size = 1;
    attr.isLive = true;
    type = INTEGER;
}

Object::Object(string val) {
    data.stringValue = val;
    attr.size = val.size();
    attr.isLive = true;
    type = STRING;
}

Object::Object(float val) {
    data.stringValue = to_string(val);
    data.realValue = val;
    data.intValue = (int) val;
    attr.isLive = true;
    attr.size = 1;
    type = REAL;
}

Object::Object(double val) {
    data.stringValue = to_string(val);
    data.realValue = val;
    data.intValue = (int) val;
    attr.isLive = true;
    attr.size = 1;
    type = REAL;
}

Object& Object::operator=(const Object& o) {
    type = o.type;
    data.intValue = o.data.intValue;
    data.realValue = o.data.realValue;
    data.stringValue = o.data.stringValue;
    attr.size = o.attr.size;
    attr.isLive = o.attr.isLive;
    return *this;
}

string Object::toString() {
    if (type == INTEGER)
        return to_string(data.intValue);
    if (type == REAL)
        return to_string(data.realValue);
    return data.stringValue;
}

#endif