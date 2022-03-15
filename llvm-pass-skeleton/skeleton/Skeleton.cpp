#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Dominators.h"

using namespace llvm;

namespace {

class GVN {
 public:
  // Constructor
  GVN(Function& F) : domTree(DominatorTree(F)) {
    this->Func = &F;
  }
  
  void runOnBlock(BasicBlock& b);

 private:
   Function* Func;
   DominatorTree domTree;
};

void GVN::runOnBlock(BasicBlock &b) {

}


struct SkeletonPass : public FunctionPass {
  static char ID;
  SkeletonPass() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    errs() << "I saw a function called " << F.getName() << "!\n";
    GVN gvn(F);
    return false;
  }
};
} // namespace

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
