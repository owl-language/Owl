#include <iostream>
#include <iomanip>
#include "../includes.hpp"
using namespace std;



int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout<<"Usage: "<<argv[0]<<" <filename> "<<endl;
        return 0;
    }
    Scanner scanner;
    scanner.openAndRead(argv[1]);
    auto lines = scanner.getLines();
    int lnw = (lines.size() < 100) ? 2:3;
    int ln = 0;
    for (auto line : lines) {
        cout<<setw(lnw)<<++ln<<": "<<line<<endl;
    }
}