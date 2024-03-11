#include "tokens.hpp"
#include <vector>
using namespace std;

void printTokens(vector<Token>& tokens) {
    for (Token token : tokens)
        token.print();
}