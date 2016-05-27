/**
 * File: symtable.h
 * ----------- 
 *  Header file for Symbol table implementation.
 */

#ifndef SYM_H_
#define SYM_H_

#include <map>
#include <vector>
#include "ast.h"
#include "ast_decl.h"
#include "irgen.h"

//using namespace std;

// Struct stores information we need
typedef struct values {
    int flag;
    llvm::Value* value;
    Decl* decl;
   
   }values;

class SymbolTable  {
    public:

        SymbolTable();
        void pop();
        int current;
        void push();
        void addSym(pair<string, values>);
        vector<map<string, values>*> *scopeVector;
        values lookupValue(int x, string s);
        values lookupValue(string s);
       
};

#endif
