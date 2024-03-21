#include "record.hpp"
using std::string;

Record::Record() {
    name = "<empty>";
}

Record::Record(string name_) {
    name = name_;
}

Record::Record(const Record& o) {
    name = o.name;
    for (auto m : o.fieldAddrs) {
        fieldAddrs[m.first] = m.second;
    }
}