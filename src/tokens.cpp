#include "tokens.hpp"
#include <vector>
using std::vector;

void printTokens(vector<Token>& tokens) {
    for (Token token : tokens)
        token.print();
}