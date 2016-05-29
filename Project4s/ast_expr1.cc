/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"
const int T = 1;
const int ZERO = 0;

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::PrintChildren(int indentLevel) { 
    printf("%d", value);
}
llvm::Value* IntConstant::Emit() {
    return llvm::ConstantInt::get(irgen->GetIntType(), value);
}

FloatConstant::FloatConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void FloatConstant::PrintChildren(int indentLevel) { 
    printf("%g", value);
}
llvm::Value* FloatConstant::Emit() {
    return llvm::ConstantFP::get(irgen->GetFloatType(), value);
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::PrintChildren(int indentLevel) { 
    printf("%s", value ? "true" : "false");
}
llvm::Value* BoolConstant::Emit() {
    return llvm::ConstantInt::get(irgen->GetBoolType(), value);
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    Assert(ident != NULL);
    this->id = ident;
}

void VarExpr::PrintChildren(int indentLevel) {
    id->Print(indentLevel+1);
}

llvm::Value* VarExpr::Emit() {
    llvm::Twine *twine = new llvm::Twine(this->id->GetName());
    llvm::Value *returnV = new llvm::LoadInst( symTable->lookupValue(id->GetName()).value, *twine, irgen->GetBasicBlock());
    return returnV; 
} 

llvm::Value* VarExpr::getValue() {
    return symTable->lookupValue(id->GetName()).value;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

bool Operator::IsOp(const char *op) const {
    return strcmp(tokenString, op) == ZERO;
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o) 
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}

void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}
   
ConditionalExpr::ConditionalExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void ConditionalExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1, "(cond) ");
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

void ArrayAccess::PrintChildren(int indentLevel) {
    base->Print(indentLevel+1);
    subscript->Print(indentLevel+1, "(subscript) ");
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}


void FieldAccess::PrintChildren(int indentLevel) {
    if (base) base->Print(indentLevel+1);
    field->Print(indentLevel+1);
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::PrintChildren(int indentLevel) {
   if (base) base->Print(indentLevel+1);
   if (field) field->Print(indentLevel+1);
   if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
}

llvm::Value* ArithmeticExpr::Emit() {
    Operator *op = this->op;
    if (this->left == NULL)
      if(this->right != NULL) {
        llvm::LoadInst *inst = llvm::cast<llvm::LoadInst>(right->Emit());
        llvm::Value *loc = inst->getPointerOperand();
        if (right->Emit()->getType()->isIntegerTy() == true) {
            if (op->IsOp("++") == true) {
                llvm::Value *inc = llvm::BinaryOperator::CreateAdd(right->Emit(), llvm::ConstantInt::get( irgen->GetIntType(), T), "int++",irgen->GetBasicBlock());
               llvm::Value *storeInst = new llvm::StoreInst(inc, loc, irgen->GetBasicBlock());
	       (void)storeInst;
                return inc;
            }
            if (op->IsOp("--")== true) {
                llvm::Value *dec = llvm::BinaryOperator::CreateSub(right->Emit(), llvm::ConstantInt::get( irgen->GetIntType(), T), "int--", irgen->GetBasicBlock());
                llvm::Value *storeInst = new llvm::StoreInst(dec, loc, irgen->GetBasicBlock());
               (void)storeInst;

	      return dec;
            }
        }
        else if (right->Emit()->getType()->isFloatTy() == true) {
            if (op->IsOp("++") == true) {
                llvm::Value *inc = llvm::BinaryOperator::CreateAdd(right->Emit(), llvm::ConstantInt::get( irgen->GetIntType(), T), "float++", irgen->GetBasicBlock());        
                llvm::Value *storeInst = new llvm::StoreInst(inc, loc, irgen->GetBasicBlock());
               (void)storeInst;

	      return inc;
            } 

            if (op->IsOp("--") == true) {
                llvm::Value *dec = llvm::BinaryOperator::CreateAdd(right->Emit(), llvm::ConstantInt::get( irgen->GetIntType(), T), "float--", irgen->GetBasicBlock());
                llvm::Value *storeInst = new llvm::StoreInst(dec, loc, irgen->GetBasicBlock());
               (void)storeInst;

	      return dec;
            }   
        }
    }

    if (this->left != NULL)
      if(this->right != NULL) {
        llvm::Type *rightType = right->Emit()->getType();
        llvm::Type *leftType = left->Emit()->getType(); 
 
        if (leftType->isIntegerTy() || rightType->isIntegerTy()) { 
             this->type = Type::intType;
             
             if (op->IsOp("+") == true) {
		return llvm::BinaryOperator::CreateAdd(left->Emit(), right->Emit(), "int+",  irgen->GetBasicBlock());
             }

            else if (op->IsOp("-") == true) {
              return llvm::BinaryOperator::CreateSub(left->Emit(), right->Emit(), "int-",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("*") == true ) {
             return llvm::BinaryOperator::CreateMul(left->Emit(), right->Emit(), "int*",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("/") == true) {
             return llvm::BinaryOperator::CreateSDiv(left->Emit(), right->Emit(), "int/",  irgen->GetBasicBlock());
            }
        }

        else if (leftType->isFloatTy() || rightType->isFloatTy()) {
            this->type = Type::floatType;
 
            if (op->IsOp("+") == true) {
             return llvm::BinaryOperator::CreateAdd(left->Emit(), right->Emit(), "float+",  irgen->GetBasicBlock());
             }

            else if (op->IsOp("-") == true) {
              return llvm::BinaryOperator::CreateSub(left->Emit(), right->Emit(), "float-",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("*") == true) {
          return llvm::BinaryOperator::CreateMul(left->Emit(), right->Emit(), "float*",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("/") == true) {
           return llvm::BinaryOperator::CreateSDiv(left->Emit(), right->Emit(), "float/",  irgen->GetBasicBlock());
            }
        }

        else if (this->left->type == Type::vec2Type || this->right->type == Type::vec2Type) {
            this->type = Type::vec2Type;            

            if (op->IsOp("+") == true) {
             return llvm::BinaryOperator::CreateFAdd(left->Emit(), right->Emit(), "vec2Add",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("-") == true) {
             return llvm::BinaryOperator::CreateFSub(left->Emit(), right->Emit(), "vec2Sub",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("*") == true) {
             return llvm::BinaryOperator::CreateFMul(left->Emit(), right->Emit(), "vec2Mult",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("/")  == true) {
             return llvm::BinaryOperator::CreateFDiv(left->Emit(), right->Emit(), "vec2Div",  irgen->GetBasicBlock());
            }
        }

        else if (this->left->type == Type::vec3Type || this->right->type == Type::vec3Type) {
            this->type = Type::vec3Type;

            if (op->IsOp("+") == true) {
             return llvm::BinaryOperator::CreateFAdd(left->Emit(), right->Emit(), "vec3Add",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("-") == true) {
             return llvm::BinaryOperator::CreateFSub(left->Emit(), right->Emit(), "vec3Sub",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("*") == true) {
             return llvm::BinaryOperator::CreateFMul(left->Emit(), right->Emit(), "vec3Mult",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("/") == true) {
             return llvm::BinaryOperator::CreateFDiv(left->Emit(), right->Emit(), "vec3Div",  irgen->GetBasicBlock());
            }
        }   

        else if (this->left->type == Type::vec4Type || this->right->type == Type::vec4Type) {
            this->type = Type::vec4Type;

            if (op->IsOp("+") == true) {
                return llvm::BinaryOperator::CreateFAdd(left->Emit(), right->Emit(), "vec4Add",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("-") == true) {
                return llvm::BinaryOperator::CreateFSub(left->Emit(), right->Emit(), "vec4Sub",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("*") == true) {
                return llvm::BinaryOperator::CreateFMul(left->Emit(), right->Emit(), "vec4Mult",  irgen->GetBasicBlock());
            }

            else if (op->IsOp("/") == true) {
                return llvm::BinaryOperator::CreateFDiv(left->Emit(), right->Emit(), "vec4Div",  irgen->GetBasicBlock());
            }
        }   
    }

    return NULL;

}

llvm::Value* PostfixExpr::Emit() {
    llvm::Type* leftType = left->Emit()->getType();
    Operator *op = this->op;

    llvm::LoadInst *inst = llvm::cast<llvm::LoadInst>(left->Emit());
    llvm::Value *loc = inst->getPointerOperand();

    llvm::BasicBlock *bb = irgen->GetBasicBlock();
    llvm::Type *intType = irgen->GetIntType();
    llvm::Value *val1 = llvm::ConstantInt::get(intType, 1);

    if (leftType->isIntegerTy()) {
        if (op->IsOp("++") == true) {
            llvm::Value *increment = llvm::BinaryOperator::CreateAdd(left->Emit(), val1, "intInc", bb);
            llvm::Value *storeInst = new llvm::StoreInst(increment, loc, bb);
            (void)storeInst;

            return inst;
        }

        else if (op->IsOp("--") == true) {
            llvm::Value *decrement = llvm::BinaryOperator::CreateSub(left->Emit(), val1, "intDec", bb);
            llvm::Value *storeInst = new llvm::StoreInst(decrement, loc, bb);
            (void)storeInst;

            return inst;
        }
    }

    else if (leftType->isFloatTy()) {
        if (op->IsOp("++")== true) {
            llvm::Value *increment = llvm::BinaryOperator::CreateAdd(left->Emit(), val1, "floatInc", bb);
            llvm::Value *storeInst = new llvm::StoreInst(increment, loc, bb);
           (void)storeInst;

            return inst;
        }

	else if (op->IsOp("--")==true) {
            llvm::Value *decrement = llvm::BinaryOperator::CreateSub(left->Emit(), val1, "floatDec", bb);
          llvm::Value *storeInst = new llvm::StoreInst(decrement, loc, bb);
              (void)storeInst;

            return inst;
        }
    } 
    return NULL;
}


llvm::Value* RelationalExpr::Emit() {
    llvm::Type* leftType = left->Emit()->getType();
    llvm::Type* rightType = right->Emit()->getType();
    Operator *op = this->op;

    if (leftType->isFloatTy() == true) {

        llvm::CmpInst::OtherOps other = llvm::CmpInst::FCmp;
        llvm::CmpInst::Predicate pred;

        if (op->IsOp(">") == true) 
          pred = llvm::CmpInst::FCMP_OGT;
        else if (op->IsOp("<") == true) 
          pred = llvm::CmpInst::FCMP_OLT;
        else if (op->IsOp(">=") == true) 
          pred = llvm::CmpInst::FCMP_OGE;
        else if (op->IsOp("<=") == true) 
          pred = llvm::CmpInst::FCMP_OLE;
        else 
          pred = llvm::CmpInst::FCMP_OLE;

        llvm::Value* res = llvm::CmpInst::Create(other, pred, left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
        return res;
    } 

    else if(rightType->isIntegerTy() == true) {
        llvm::CmpInst::OtherOps other = llvm::CmpInst::ICmp;
        llvm::CmpInst::Predicate pred;

        if (op->IsOp(">") == true) 
          pred = llvm::CmpInst::ICMP_SGT;
        else if (op->IsOp("<")== true) 
          pred = llvm::CmpInst::ICMP_SLT;
        else if (op->IsOp(">=") == true) 
          pred = llvm::CmpInst::ICMP_SGE;
        else if ( op->IsOp("<=") == true)
          pred = llvm::CmpInst::ICMP_SLE;    
        else 
          pred = llvm::CmpInst::ICMP_SLE;

        llvm::Value* res = llvm::CmpInst::Create(other, pred, left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
        return res;
    }
  return NULL;
}

llvm::Value* EqualityExpr::Emit() {
    llvm::Type* rightType = right->Emit()->getType();
    llvm::Type* leftType = left->Emit()->getType();
    Operator *op = this->op;

    if (leftType->isIntegerTy() == true) {
        llvm::CmpInst::OtherOps other = llvm::CmpInst::ICmp;
        llvm::CmpInst::Predicate pred = llvm::CmpInst::FCMP_FALSE;
        string str = "";

        if (op->IsOp("==") == true) {
            pred = llvm::CmpInst::ICMP_EQ;
            str = "intEq";
        }

        else if (op->IsOp("!=") == true) {
            pred = llvm::CmpInst::ICMP_NE;
            str = "intNotEq";   
        }

  return llvm::CmpInst::Create(other, pred, left->Emit(), right->Emit(), str, irgen->GetBasicBlock());
    }

    else if (rightType->isIntegerTy() == true)
    {
      llvm::CmpInst::OtherOps other = llvm::CmpInst::ICmp;
        llvm::CmpInst::Predicate pred = llvm::CmpInst::FCMP_FALSE;
        string str = "";

        if (op->IsOp("==") == true) {
            pred = llvm::CmpInst::ICMP_EQ;
            str = "intEq";
        }

        else if (op->IsOp("!=") == true) {
            pred = llvm::CmpInst::ICMP_NE;
            str = "intNotEq";   
        }

    return llvm::CmpInst::Create(other, pred, left->Emit(), right->Emit(), str, irgen->GetBasicBlock());
    
    }

    else if (leftType->isFloatTy() == true) {
        llvm::CmpInst::OtherOps other = llvm::FCmpInst::FCmp;
        llvm::CmpInst::Predicate pred = llvm::CmpInst::FCMP_FALSE;
        string str = "";

        if (op->IsOp("==") == true) {
            pred = llvm::FCmpInst::FCMP_OEQ;
            str = "floatEq";
        }

        else if (op->IsOp("!=") == true) {
            pred = llvm::FCmpInst::FCMP_ONE;
            str = "floatNotEq";
        }

        llvm::Value* res = llvm::CmpInst::Create(other, pred, left->Emit(), right->Emit(), str, irgen->GetBasicBlock());
        return res;
    }
    else if (rightType->isFloatTy())
    {
         llvm::CmpInst::OtherOps other = llvm::FCmpInst::FCmp;
        llvm::CmpInst::Predicate pred = llvm::CmpInst::FCMP_FALSE;
        string str = "";

        if (op->IsOp("==") == true) {
            pred = llvm::FCmpInst::FCMP_OEQ;
            str = "floatEq";
        }

        else if (op->IsOp("!=") == true) {
            pred = llvm::FCmpInst::FCMP_ONE;
            str = "floatNotEq";
        }

        llvm::Value* res = llvm::CmpInst::Create(other, pred, left->Emit(), right->Emit(), str, irgen->GetBasicBlock());
        return res;

    
    
    
    }
    return NULL;
}

llvm::Value* LogicalExpr::Emit() {
    Operator* op = this->op;

    if (op->IsOp("||")) {
      return llvm::BinaryOperator::CreateOr(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
    }
    else if (op->IsOp("&&")) {
      return  llvm::BinaryOperator::CreateAnd(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
    }
    

    return NULL;
}

llvm::Value* AssignExpr::Emit() {
    llvm::Value* lhs;
    llvm::Value* lhsVal;
    const char* swizzle = "";

    if (VarExpr* lhv = dynamic_cast<VarExpr*>(left)) 
        lhsVal = lhv->getValue();
    else if (FieldAccess *field = dynamic_cast<FieldAccess*>(left)) {
        lhsVal = field->getValue();
        swizzle = field->getFieldId()->GetName();
    }
    else 
        lhsVal = right->Emit();

    if (llvm::StoreInst* storeInst = dynamic_cast<llvm::StoreInst*>(right->Emit())) {
        lhs = storeInst->getValueOperand();
    }

    llvm::Type* leftType;
    llvm::Type* rightType = right->Emit()->getType();
    Operator * op = this->op;
    int lenght = strlen(swizzle);

    if (op->IsOp("=")) {
        if (lenght != ZERO) {
            llvm::Value* baseAdd = new llvm::LoadInst(lhsVal, "", irgen->GetBasicBlock());
            llvm::Constant* id;

            if (rightType->isVectorTy()) {
	     int i = ZERO;
                while ( i < lenght) {
                    char c = swizzle[i];
                    switch(c){
                    case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                    case 'y':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                    case 'z':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                    default :
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Constant* idx = llvm::ConstantInt::get(irgen->GetIntType(), i);
                    llvm::Value* extract = llvm::ExtractElementInst::Create(right->Emit(), idx, "", irgen->GetBasicBlock());
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, extract, id, "", irgen->GetBasicBlock());
		    i++;
                }
            }

            else if (rightType->isFloatTy()) {
	      int i = ZERO;
                while ( i < lenght) {
                    char c = swizzle[i];
                    switch(c)
		    {
                    case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                    case 'y': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                    case 'z':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                    default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                   }
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, right->Emit(), id, "", irgen->GetBasicBlock());
		    i = i+1;
                }
            }

            llvm::Value* res = new llvm::StoreInst(baseAdd, lhsVal, "", irgen->GetBasicBlock());
            (void) res;
	    return right->Emit();
        }

        llvm::Value* res = new llvm::StoreInst(right->Emit(), lhsVal, irgen->GetBasicBlock());
        (void) res;
	return right->Emit();
    }

    else if (op->IsOp("+=")) {
        if(lenght != ZERO) {
            llvm::Value* baseAdd = new llvm::LoadInst(lhsVal, "", irgen->GetBasicBlock());
            llvm::Constant* id;

            if (rightType->isVectorTy()) {
	        int i = 0;
                while ( i < lenght) {
                    char c = swizzle[i];
                    switch (c){
                     case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                     case 'y':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                     case 'z':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                     default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Constant* idx = llvm::ConstantInt::get(irgen->GetIntType(), i);
                    llvm::Value* extractrhs = llvm::ExtractElementInst::Create(right->Emit(), idx, "", irgen->GetBasicBlock());
                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFAdd(extractlhs, extractrhs, "", irgen->GetBasicBlock());

                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i++;
                }
            }

            else if (rightType->isFloatTy()) {
	       int i = ZERO;
                while ( i < lenght) {
                    char c = swizzle[i];
                    switch (c){
                     case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                     case 'y': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                     case 'z': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                     default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFAdd(extractlhs, right->Emit(), "", irgen->GetBasicBlock());
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i++;
                }

                llvm::Value* res = new llvm::StoreInst(baseAdd, lhsVal, "", irgen->GetBasicBlock());
               (void) res;
	       return right->Emit();
            }
        }

        lhs = left->Emit();
        leftType = left->Emit()->getType();

        if(leftType->isFloatTy() || leftType->isVectorTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateFAdd(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }

        else if (leftType->isIntegerTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateAdd(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }
    }

    else if (op->IsOp("-=")) {
        if(lenght != ZERO) {
            llvm::Value* baseAdd = new llvm::LoadInst(lhsVal, "", irgen->GetBasicBlock());
            llvm::Constant* id;

            if (rightType->isVectorTy()) {
                int i = ZERO;
                while ( i < lenght) {
                    char c = swizzle[i];
                    switch(c){
                    case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                    case 'y': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                    case 'z': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                    default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Constant* idx = llvm::ConstantInt::get(irgen->GetIntType(), i);
                    llvm::Value* extractrhs = llvm::ExtractElementInst::Create(right->Emit(), idx, "", irgen->GetBasicBlock());
                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFSub(extractlhs, extractrhs, "", irgen->GetBasicBlock());

                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i++;
                }
            }

            else if (rightType->isFloatTy()) {
	      int i = ZERO;
                while (i < lenght) {
                    char option = swizzle[i];
                    switch(option)
		    {
                    case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                    case'y':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                    case 'z': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                    default: 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFSub(extractlhs, right->Emit(), "", irgen->GetBasicBlock());
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i++;
                }

                llvm::Value* res = new llvm::StoreInst(baseAdd, lhsVal, "", irgen->GetBasicBlock());
		(void) res;
                return right->Emit();
            }
        }

        lhs = left->Emit();
        leftType = left->Emit()->getType();

        if(leftType->isFloatTy() || leftType->isVectorTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateFSub(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }

        else if (leftType->isIntegerTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateSub(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }
    }

    else if (op->IsOp("*=")) {
        if (lenght != ZERO) {
            llvm::Value* baseAdd = new llvm::LoadInst(lhsVal, "", irgen->GetBasicBlock());
            llvm::Constant* id;

            if (rightType->isVectorTy()) {
                int step = ZERO;
                while (step < lenght) {
                    char c = swizzle[step];
                    switch(c){
                    case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                    case 'y': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                    case 'z':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                    default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
		    }	

                    llvm::Constant* idx = llvm::ConstantInt::get(irgen->GetIntType(), step);
                    llvm::Value* extractrhs = llvm::ExtractElementInst::Create(right->Emit(), idx, "", irgen->GetBasicBlock());
                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFMul(extractlhs, extractrhs, "", irgen->GetBasicBlock());

                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    step++;
                }
            }

            else if (rightType->isFloatTy()) {
	      int i = ZERO; 
                while (i < lenght) {
                    char option = swizzle[i];
                    switch(option){
                     case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                      case 'y':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                      case 'z': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                      default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
		    }	

                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFMul(extractlhs, right->Emit(), "", irgen->GetBasicBlock());
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i++;
                }

                llvm::Value* res = new llvm::StoreInst(baseAdd, lhsVal, "", irgen->GetBasicBlock());
                (void) res;
		return right->Emit();
            }
        }

        lhs = left->Emit();
        leftType = left->Emit()->getType();

        if(leftType->isFloatTy() || leftType->isVectorTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateFMul(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }

        else if (leftType->isIntegerTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateMul(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }
    }

    else if (op->IsOp("/=")) {
        if (lenght != ZERO) {
            llvm::Value* baseAdd = new llvm::LoadInst(lhsVal, "", irgen->GetBasicBlock());
            llvm::Constant* id;

            if (rightType->isVectorTy()) {
	        int i = ZERO;
                while ( i < lenght) {
                    char c = swizzle[i];
                    switch(c){
                     case 'x':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                     case 'y': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                     case 'z': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                     default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Constant* idx = llvm::ConstantInt::get(irgen->GetIntType(), i);
                    llvm::Value* extractrhs = llvm::ExtractElementInst::Create(right->Emit(), idx, "", irgen->GetBasicBlock());
                    llvm::Value* extractlhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFDiv(extractlhs, extractrhs, "", irgen->GetBasicBlock());
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i = i+1;
                }
            }

            else if (rightType->isFloatTy()) {
	        int i = ZERO;
                while ( i < lenght) {
                    char option = swizzle[i];
                    switch(option){
                    case 'x': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                    case 'y': 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                    case 'z':
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                    default : 
                        id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                    }
                    llvm::Value* elhs = llvm::ExtractElementInst::Create(baseAdd, id, "", irgen->GetBasicBlock());
                    llvm::Value* binaryOp = llvm::BinaryOperator::CreateFDiv(elhs, right->Emit(), "", irgen->GetBasicBlock());
                    baseAdd = llvm::InsertElementInst::Create(baseAdd, binaryOp, id, "", irgen->GetBasicBlock());
		    i++;
                }

                llvm::Value* res = new llvm::StoreInst(baseAdd, lhsVal, "", irgen->GetBasicBlock());
                (void) res;
		return right->Emit();
            }
        }

        lhs = left->Emit();
        leftType = left->Emit()->getType();

        if(leftType->isFloatTy() || leftType->isVectorTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateFDiv(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }

        else if (leftType->isIntegerTy()) {
            llvm::Value* res = llvm::BinaryOperator::CreateSDiv(left->Emit(), right->Emit(), "", irgen->GetBasicBlock());
            new llvm::StoreInst(res, lhsVal, irgen->GetBasicBlock());
            return res;
        }
    }    
    
    return NULL; 
    
} 

llvm::Value* FieldAccess::Emit() {
    llvm::Value* lhs = base->Emit();
    const char* fieldName = field->GetName();

    std::vector<llvm::Constant*> consts;

    int fieldLength = strlen(fieldName);

    if (fieldLength == 1) {
        char option = fieldName[ZERO];
     
    switch (option) {

     case 'c' :
     {

            llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
            llvm::Value* res = llvm::ExtractElementInst::Create(base->Emit(), id, "", irgen->GetBasicBlock());
            return res;
        }

      case 'y':
      
      {
            llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
            llvm::Value* res = llvm::ExtractElementInst::Create(base->Emit(), id, "", irgen->GetBasicBlock());
            return res;
        }

       case 'z': {
            llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
            llvm::Value* res = llvm::ExtractElementInst::Create(base->Emit(), id, "", irgen->GetBasicBlock());
            return res;
        }

       default : {
            llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
            llvm::Value* res = llvm::ExtractElementInst::Create(base->Emit(), id, "", irgen->GetBasicBlock());
            return res;
     
       }
      }
    }

    else {
        int i = ZERO;
        while ( i < fieldLength) {
            char option = fieldName[i];
            switch(option)
	    {
            case 'x': {
                llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), ZERO);
                consts.push_back(id);
            }

            case 'y': {
                llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), 1);
                consts.push_back(id);
            }

            case 'z': {
                llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), 2);
                consts.push_back(id);
            }

            default: {
                llvm::Constant* id = llvm::ConstantInt::get(irgen->GetIntType(), 3);
                consts.push_back(id);
             }
	    }
	  i = i + 1;  
        }

        llvm::ConstantVector* cv = (llvm::ConstantVector*)llvm::ConstantVector::get(consts);
        llvm::Value* res = new llvm::ShuffleVectorInst(lhs, llvm::UndefValue::get(
            lhs->getType()), cv, "", irgen->GetBasicBlock());
        return res;
    }
}

llvm::Value* FieldAccess::getValue() {
    //const char* c = field->GetName();

    if (VarExpr* v = dynamic_cast<VarExpr*>(base)) {
        return v->getValue();
    }

    else if (FieldAccess* f = dynamic_cast<FieldAccess*>(base)) {
        return f->getValue();
    }

    else return base->getValue();
}
