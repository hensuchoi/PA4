/* irgen.cc -  LLVM IR generator
 *
 * You can implement any LLVM related functions here.
 */

#include "irgen.h"

IRGenerator::IRGenerator() :
    context(NULL),
    module(NULL),
    currentFunc(NULL),
    currentBB(NULL)
{
}

IRGenerator::~IRGenerator() {
}

llvm::Module *IRGenerator::GetOrCreateModule(const char *moduleID)
{
   if ( module == NULL ) {
     context = new llvm::LLVMContext();
     module  = new llvm::Module(moduleID, *context);
     module->setTargetTriple(TargetTriple);
     module->setDataLayout(TargetLayout);
   }
   return module;
}

void IRGenerator::SetFunction(llvm::Function *func) {
   currentFunc = func;
}

llvm::Function *IRGenerator::GetFunction() const {
   return currentFunc;
}

void IRGenerator::SetBasicBlock(llvm::BasicBlock *basicBlock) {
   currentBB = basicBlock;
}

llvm::BasicBlock *IRGenerator::GetBasicBlock() const {
   return currentBB;
}

llvm::Type *IRGenerator::GetIntType() const {
   llvm::Type *ty = llvm::Type::getInt32Ty(*context);
   return ty;
}

llvm::Type *IRGenerator::GetBoolType() const {
   llvm::Type *ty = llvm::Type::getInt1Ty(*context);
   return ty;
}

llvm::Type *IRGenerator::GetFloatType() const {
   llvm::Type *ty = llvm::Type::getFloatTy(*context);
   return ty;
}

/* Conversion function */
llvm::Type *IRGenerator::convertType(Type *ty, llvm::LLVMContext *context) {
   llvm::Type *t = NULL;
   if (ty == Type::intType) {
      t = llvm::Type::getInt32Ty(*context);
   }

   else if (ty == Type::boolType) {
      t = llvm::Type::getInt1Ty(*context);
   }

   else if (ty == Type::floatType) {
      t = llvm::Type::getFloatTy(*context);
   }

   //More types TODO here - Vec2/3/4, Mat2/3/4

   return t;

}



/*llvm::Type *IRGenerator::GetVec2Type() const {
   llvm::Type *ty = llvm::VectorType::get(llvm::Type::getFloatTy(*context), 2);
   return ty;
}

llvm::Type *IRGenerator::GetVec3Type() const {
   llvm::Type *ty = llvm::VectorType::get(llvm::Type::getFloatTy(*context), 3);
   return ty;
}

llvm::Type *IRGenerator::GetVec4Type() const {
   llvm::Type *ty = llvm::VectorType::get(llvm::Type::getFloatTy(*context), 4);
   return ty;
}

llvm::Type *IRGenerator::GetMat2Type() const {
   llvm::Type *elementTy = llvm::VectorType::get(llvm::Type::getFloatTy(*context), 2);
   llvm::Type *ty = llvm::ArrayType::get(elementTy, 2);
   return ty;
}

llvm::Type *IRGenerator::GetMat3Type() const {
   llvm::Type *elementTy = llvm::VectorType::get(llvm::Type::getFloatTy(*context), 2);
   llvm::Type *ty = llvm::ArrayType::get(elementTy, 3);
   return ty;

}

llvm::Type *IRGenerator::GetMat4Type() const {
   llvm::Type *elementTy = llvm::VectorType::get(llvm::Type::getFloatTy(*context), 2);
   llvm::Type *ty = llvm::ArrayType::get(elementTy, 4);
   return ty;

}*/

const char *IRGenerator::TargetLayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128";

const char *IRGenerator::TargetTriple = "x86_64-redhat-linux-gnu";

