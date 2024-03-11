#include "tokens.hpp"

void printTokens(vector<Token>& tokens) {
    for (Token token : tokens)
        token.print();
}