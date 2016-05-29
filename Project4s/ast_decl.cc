/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"        
#include "irgen.h"
const int P = 1;
const bool F = false;
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

VarDecl::VarDecl(Identifier *n, Type *t, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    typeq = NULL;
}

VarDecl::VarDecl(Identifier *n, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && tq != NULL);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    type = NULL;
}

VarDecl::VarDecl(Identifier *n, Type *t, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL && tq != NULL);
    (type=t)->SetParent(this);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
}
  
void VarDecl::PrintChildren(int indentLevel) { 
   if (typeq) typeq->Print(indentLevel+P);
   if (type) type->Print(indentLevel+P);
   if (id) id->Print(indentLevel+P);
   if (assignTo) assignTo->Print(indentLevel+P, "(initializer) ");
}

llvm::Value* VarDecl::Emit() {
    llvm::Twine *vName = new llvm::Twine(this->id->GetName());
    llvm::Type *type = IRGenerator::convertType(this->GetType(), irgen->GetContext());
    values in;

    if (symTable->current != P) {
        llvm::BasicBlock *bb = irgen->GetBasicBlock();
        llvm::Value *val = new llvm::AllocaInst( IRGenerator::convertType(this->GetType(), irgen->GetContext()), *vName, bb);
        in.value = val;
        in.decl = this;
        in.flag = 0;
   }
    
     if (symTable->current == P){
        llvm::GlobalVariable *gvar = new llvm::GlobalVariable(
            *irgen->GetOrCreateModule("module.bc"),  IRGenerator::convertType(this->GetType(), irgen->GetContext()), F, llvm::GlobalValue::ExternalLinkage, llvm::Constant::getNullValue( IRGenerator::convertType(this->GetType(), irgen->GetContext())), *vName);
        in.value = gvar;
        in.decl = this;
        in.flag = P;  
     } 
    symTable->addSym(make_pair(getId(), in));
    return NULL;
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
    returnTypeq = NULL;
}

FnDecl::FnDecl(Identifier *n, Type *r, TypeQualifier *rq, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r != NULL && rq != NULL&& d != NULL);
    (returnType=r)->SetParent(this);
    (returnTypeq=rq)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+P, "(return type) ");
    if (id) id->Print(indentLevel+P);
    if (formals) formals->PrintAll(indentLevel+P, "(formals) ");
    if (body) body->Print(indentLevel+P, "(body) ");
}


llvm::Value* FnDecl::Emit() {
    symTable->push();
    vector<llvm::Type*> vType;
    llvm::Type *ty = IRGenerator::convertType(returnType, irgen->GetContext());
    int i = 0;
    while ( i < formals->NumElements()) {
       Type *tp = formals->Nth(i)->GetType();
       llvm::Type* type = IRGenerator::convertType(tp, irgen->GetContext());
       vType.push_back(type);
       i++;
    }

    llvm::ArrayRef<llvm::Type*> arrayV(vType);
    llvm::FunctionType *funcType = llvm::FunctionType::get(ty, arrayV, F);    
    irgen->SetFunction(llvm::cast<llvm::Function>(
        irgen->GetOrCreateModule("foo.bc")->getOrInsertFunction(llvm::StringRef(this->id->GetName()), funcType)));
    const llvm::Twine* name = new llvm::Twine(this->id->GetName());
    llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create( *irgen->GetContext(), *name, llvm::cast<llvm::Function>(
        irgen->GetOrCreateModule("foo.bc")->getOrInsertFunction(llvm::StringRef(this->id->GetName()), funcType)));
    irgen->SetBasicBlock(basicBlock);
    int j = 0;
    llvm::Function::arg_iterator iter = llvm::cast<llvm::Function>(
        irgen->GetOrCreateModule("foo.bc")->getOrInsertFunction(llvm::StringRef(this->id->GetName()), funcType))->arg_begin();
    while ( iter != llvm::cast<llvm::Function>(
        irgen->GetOrCreateModule("foo.bc")->getOrInsertFunction(llvm::StringRef(this->id->GetName()), funcType))->arg_end()) {
        formals->Nth(i)->Emit();
        iter->setName( formals->Nth(i)->getId());
        values in = symTable->lookupValue( formals->Nth(i)->getId());
        new llvm::StoreInst((&*iter), in.value, basicBlock);
	iter++;
	j++;
    }

    body->Emit();
    symTable->pop();

    return NULL;
} 

