/*
 * Symbol table implementation
 *
 */

#include "symtable.h"
#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
const int  P = 1;

SymbolTable::SymbolTable() {
    scopeVector = new vector<map<string, values>*>();
    current = 0;
}

void SymbolTable::pop() {
    scopeVector->pop_back();
    current = current - P;
}
void SymbolTable::push() {
    scopeVector->push_back(new map<string,values>());
    current = current + P;
}


void SymbolTable::addSym(pair<string, values> s) {
    
    int curr = current - P;
    if (lookupValue( curr, s.first).flag != -P)
        return;
    scopeVector->at(current - P)->insert(s);
}

values SymbolTable::lookupValue( int x, string s) {
    map<string, values> *currMap = scopeVector->at(x);
    map<string, values>::iterator it;
    it = currMap->find(s);

    if (it != currMap->end()) {
        return it->second;
    }

    values none = { -P, NULL, NULL };
    return none;
}

values SymbolTable::lookupValue(string s) {
   int i = current - P;
    while ( i >= 0) {
        values in = lookupValue( i,s);

        if (in.flag != -P) {
            return in;
        }
	i= i - P;

    }
    
    values none = {-P, NULL, NULL};
    return none;
}


