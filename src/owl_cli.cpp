#include <iostream>
#include <map>
#include <cstring>
#include "cli/cli.hpp"
#include "repl/repl.hpp"
using std::cout;
using std::endl;

void showUsage() {
    cout<<"[ Owl v"<<OWL_VERSION<<" "<<FLACO<<" ]"<<endl;
    cout<<"Usage: "<<endl;
    cout<<"      owl -r to launch REPL"<<endl;
    cout<<"      owl <options> <filename> to load Owl code from file."<<endl;
    cout<<"Optional Flags: "<<endl;
    cout<<"       -h   display this help message."<<endl;
    cout<<"       -v   display tokens and generated AST"<<endl;
    cout<<"       -vp  include trace of parse step"<<endl;
    cout<<"       -vi  show trace of interpreter"<<endl;
    cout<<"       -vip show all of the above"<<endl;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (argc < 2) {
        showUsage();
        return 0;
    }
    OwlCLI owlCLI;
    OwlREPL OwlREPL;
    if (argv[1][0] == '-') {
        switch (argv[1][1]) {
            case 'h':
                showUsage();
                break;
            case 'r':
                if (argv[1][2] == 'v') {
                    initTracer("-vi");
                }
                OwlREPL.repl();
                break;
            case 'v':
                owlCLI.runVerbose(argv[1], argv[2]);
                break;
            default:
                cout<<"Unknown option: "<<argv[1]<<endl;
                return 0;
        }
    } else {
        owlCLI.runQuiet(argv[1]);
    }
    return 0;
}