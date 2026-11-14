#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"


int main() {
    llvm::LLVMContext ctx;
    llvm::Module mod("lisan", ctx);
    llvm::IRBuilder<> b(ctx);

    auto* fnTy = llvm::FunctionType::get(b.getInt64Ty(), /*isVarArg*/false);
    auto* fn = llvm::Function::Create(fnTy, llvm::Function::ExternalLinkage, "main", &mod);
    /*
        ExternalLinkage: globally visible symbol
        InternalLinkage: private to the llvm module
        PrivateLinkage: Module-local symbol with more restrictive symbol visibility
    */

    auto* entry = llvm::BasicBlock::Create(ctx, "entry", fn);

    b.SetInsertPoint(entry);          // “append new instructions here”

    llvm::Value* sum = b.CreateAdd(b.getInt64(40), b.getInt64(2), "sum");
    b.CreateRet(sum);                  // the terminator

    llvm::verifyFunction(*fn, &llvm::errs());   // prints problems, returns true if broken
    mod.print(llvm::outs(), nullptr);


}