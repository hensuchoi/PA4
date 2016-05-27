/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "symtable.h"

#include "irgen.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"                                                   


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

llvm::Value* Program::Emit() {
    llvm::Module *mod = irgen->GetOrCreateModule("mod.bc");

    symTable->push();

    for (int i = 0; i < decls->NumElements(); i++) {
        Decl *d = decls->Nth(i);
        d->Emit();
    }

    symTable->pop();
    mod->dump();
    llvm::WriteBitcodeToFile(mod, llvm::outs());

    return NULL;
}



StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

llvm::Value* StmtBlock::Emit() {

    symTable->push();


    for (int i = 0; i < decls->NumElements(); i++) {
        decls->Nth(i);
    }

    for (int i = 0; i < stmts->NumElements(); i++) {
        if (!irgen->GetBasicBlock()->getTerminator()) {
            stmts->Nth(i)->Emit();
        }
    }


    symTable->pop();

    return NULL;

}

DeclStmt::DeclStmt(Decl *d) {
    Assert(d != NULL);
    (decl=d)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    decl->Print(indentLevel+1);
}

llvm::Value* DeclStmt::Emit() {
    decl->Emit();

    return NULL;
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && b != NULL);
    (init=i)->SetParent(this);
    step = s;
    if ( s )
      (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    if ( step )
      step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

llvm::Value* ForStmt::Emit() {
    llvm::Function *func = irgen->GetFunction();
    symTable->push();
    init->Emit();

    llvm::BasicBlock *currBB = irgen->GetBasicBlock();

    llvm::Twine *footerTwine = new llvm::Twine("footer");
    llvm::Twine *stepTwine = new llvm::Twine("step");
    llvm::Twine *bodyTwine = new llvm::Twine("body");

    llvm::BasicBlock* footerBB = llvm::BasicBlock::Create(*irgen->GetContext(), *footerTwine, irgen->GetFunction());
    llvm::BasicBlock* stepBB = llvm::BasicBlock::Create(*irgen->GetContext(), *stepTwine, irgen->GetFunction());
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*irgen->GetContext(), *bodyTwine, irgen->GetFunction());

    llvm::BranchInst::Create(bodyBB, footerBB, test->Emit(), currBB);

    bodyBB->moveAfter(irgen->GetBasicBlock());
    irgen->SetBasicBlock(bodyBB);
    body->Emit();
    if(irgen->GetBasicBlock()->getTerminator() == NULL)
        llvm::BranchInst::Create(stepBB, irgen->GetBasicBlock());

    stepBB->moveAfter(irgen->GetBasicBlock());
    irgen->SetBasicBlock(stepBB);
    step->Emit();
    if(irgen->GetBasicBlock()->getTerminator() == NULL) 
        llvm::BranchInst::Create(bodyBB, footerBB, test->Emit(), irgen->GetBasicBlock());

    footerBB->moveAfter(irgen->GetBasicBlock());

    if(irgen->GetBasicBlock()->getTerminator() == NULL)
        llvm::BranchInst::Create(stepBB, irgen->GetBasicBlock());

    irgen->SetBasicBlock(footerBB); 
    symTable->pop();

    return NULL;
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

llvm::Value* WhileStmt::Emit() {
    llvm::Function *func = irgen->GetFunction();
    llvm::BasicBlock *currBB = irgen->GetBasicBlock();

    llvm::Twine *testTwine = new llvm::Twine("test");
    llvm::Twine *footerTwine = new llvm::Twine("footer");
    llvm::Twine *bodyTwine = new llvm::Twine("body");

    llvm::BasicBlock* testBB = llvm::BasicBlock::Create(*irgen->GetContext(), *testTwine, irgen->GetFunction());
    llvm::BasicBlock* footerBB = llvm::BasicBlock::Create(*irgen->GetContext(), *footerTwine, irgen->GetFunction());
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*irgen->GetContext(), *bodyTwine, irgen->GetFunction());
    
    symTable->push();

    if (currBB->getTerminator() == NULL) 
        llvm::BranchInst::Create(testBB, currBB);

    testBB->moveAfter(irgen->GetBasicBlock());

    irgen->SetBasicBlock(testBB);

    if (testBB->getTerminator() == NULL) 
        llvm::BranchInst::Create(bodyBB, footerBB, test->Emit(), testBB);

    bodyBB->moveAfter(testBB);
    irgen->SetBasicBlock(bodyBB);
    body->Emit();

    if(bodyBB->getTerminator() == NULL) 
        llvm::BranchInst::Create(testBB, bodyBB);

    footerBB->moveAfter(irgen->GetBasicBlock());

    if(irgen->GetBasicBlock()->getTerminator() == NULL)
        llvm::BranchInst::Create(testBB, irgen->GetBasicBlock());

    irgen->SetBasicBlock(footerBB);
    symTable->pop();

    return NULL;
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}

llvm::Value* IfStmt::Emit() {
    
    symTable->push();
    llvm::Function *func = irgen->GetFunction();

    llvm::Twine *footerTwine = new llvm::Twine("if_footer");
    llvm::Twine *elseTwine = new llvm::Twine("else");
    llvm::Twine *thenTwine = new llvm::Twine("then");

    llvm::BasicBlock *headerBB = irgen->GetBasicBlock();
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*irgen->GetContext(), *thenTwine, irgen->GetFunction());
    llvm::BasicBlock *elseBB = NULL;

    if (elseBody) {
        elseBB = llvm::BasicBlock::Create(*irgen->GetContext(), *elseTwine, irgen->GetFunction());
    }

    llvm::BasicBlock *footerBB = llvm::BasicBlock::Create(*irgen->GetContext(), *footerTwine, irgen->GetFunction());

    llvm::BranchInst::Create(thenBB, elseBody ? elseBB : footerBB, test->Emit(), headerBB);

    thenBB->moveAfter(headerBB);
    irgen->SetBasicBlock(thenBB);
    body->Emit();

    if (!thenBB->getTerminator()) {
        llvm::BranchInst::Create(footerBB, thenBB);
    }

    if (!irgen->GetBasicBlock()->getTerminator()) {
        llvm::BranchInst::Create(footerBB, irgen->GetBasicBlock());
    }

    if (elseBody) {
        elseBB->moveAfter(irgen->GetBasicBlock());
        irgen->SetBasicBlock(elseBB);
        elseBody->Emit();

        if (!elseBB->getTerminator()) {
            llvm::BranchInst::Create(footerBB, elseBB);
        }

        irgen->SetBasicBlock(elseBB);
    }

    footerBB->moveAfter(irgen->GetBasicBlock());

    if (!irgen->GetBasicBlock()->getTerminator()) {
        llvm::BranchInst::Create(footerBB, irgen->GetBasicBlock());
    }

    irgen->SetBasicBlock(footerBB);

    symTable->pop();

    return NULL;
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    expr = e;
    if (e != NULL) expr->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    if ( expr ) 
      expr->Print(indentLevel+1);
}

llvm::Value* ReturnStmt::Emit() {
    llvm::BasicBlock* bb = irgen->GetBasicBlock();
    if (expr ) {
        llvm::Value* rval = expr->Emit();
        llvm::ReturnInst::Create(*irgen->GetContext(), rval, bb);
    }

    else {
        llvm::ReturnInst::Create(*irgen->GetContext(), bb);
    }

    return NULL;
}

SwitchLabel::SwitchLabel(Expr *l, Stmt *s) {
    Assert(l != NULL && s != NULL);
    (label=l)->SetParent(this);
    (stmt=s)->SetParent(this);
}

SwitchLabel::SwitchLabel(Stmt *s) {
    Assert(s != NULL);
    label = NULL;
    (stmt=s)->SetParent(this);
}

void SwitchLabel::PrintChildren(int indentLevel) {
    if (label) label->Print(indentLevel+1);
    if (stmt)  stmt->Print(indentLevel+1);
}

SwitchStmt::SwitchStmt(Expr *e, List<Stmt *> *c, Default *d) {
    Assert(e != NULL && c != NULL && c->NumElements() != 0 );
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
    def = d;
    if (def) def->SetParent(this);
}

void SwitchStmt::PrintChildren(int indentLevel) {
    if (expr) expr->Print(indentLevel+1);
    if (cases) cases->PrintAll(indentLevel+1);
    if (def) def->Print(indentLevel+1);
}


llvm::Value* SwitchStmt::Emit() {
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*irgen->GetContext(), "body", irgen->GetFunction());
    llvm::BasicBlock *footerBB = llvm::BasicBlock::Create(*irgen->GetContext(), "footer", irgen->GetFunction());
    llvm::BasicBlock *defaultBB = llvm::BasicBlock::Create(*irgen->GetContext(), "default", irgen->GetFunction());
    llvm::BasicBlock *caseBB = llvm::BasicBlock::Create(*irgen->GetContext(), "case", irgen->GetFunction());

    llvm::SwitchInst* sw = llvm::SwitchInst::Create(expr->Emit(), defaultBB, cases->NumElements(), irgen->GetBasicBlock());	
   int i =0;
    while (i < cases->NumElements()) {
        if (dynamic_cast<Case*>(cases->Nth(i))) {
             irgen->SetBasicBlock(caseBB);
             llvm::Value* label = dynamic_cast<Case*>(cases->Nth(i))->returnLabel()->Emit();
             llvm::ConstantInt *constLabel = llvm::cast<llvm::ConstantInt>(label);
             llvm::BranchInst::Create(caseBB, irgen->GetBasicBlock());
             cases->Nth(i)->Emit();
        }

        else if (dynamic_cast<Default*>(cases->Nth(i))) {
       
             irgen->SetBasicBlock(defaultBB);
             llvm::BranchInst::Create(defaultBB, irgen->GetBasicBlock());
             cases->Nth(i)->Emit();

        }
	i++;
    }

    if (defaultBB->getTerminator() == NULL) {
        llvm::BranchInst::Create(footerBB, defaultBB);
    }

    irgen->SetBasicBlock(footerBB);
    return NULL;
}

llvm::Value* BreakStmt::Emit() {
    llvm::BranchInst::Create(breakBB->back(), irgen->GetBasicBlock()); 
    return NULL;    
}

llvm::Value* ContinueStmt::Emit() {
    llvm::BranchInst::Create(continueBB->back(), irgen->GetBasicBlock());
    return NULL;
}

