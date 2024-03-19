#include "memobject.hpp"

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