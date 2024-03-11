#include "filebuffer.hpp"

bool FileReader::readFile(string filename) {
    infile.open(filename, ios::in);
    if (!infile.is_open()) {
        cout<<"Couldnt open file: "<<filename<<" for reading."<<endl;
        return false;
    }
    string line = "";
    while (nextchar()) {
        if (readChar == '\0' || readChar == '\r' || readChar == '\n') {
            if (line.empty()) {
                continue;
            }
            linenum++;
            lines.push_back(line);
            line.clear();
        } else {
            line.push_back(readChar);
        }
    }
    lines.push_back(line);
    infile.close();
    return true;
}

