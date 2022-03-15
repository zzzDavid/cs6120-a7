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

  void run(); 
  void runOnBlock(BasicBlock& b);

 private:
  Function* Func;
  DominatorTree domTree;
  // env maps symbol name to value number
  std::map<std::string, unsigned> env;
  // value numbering table, implemented as a list
  // [(opcode, [operands])]
  std::vector<std::pair<unsigned, std::vector<Value*>>> table;
};

void GVN::runOnBlock(BasicBlock &B) {
  // print all instructions
  llvm::outs() << B.getName() << "\n";
  for (auto &I : B) {
      llvm::outs() << I << "\n";
  }
  llvm::outs() << "\n";
}

void GVN::run() {
  for (auto &B : *this->Func) {
    runOnBlock(B);
  }
}

struct SkeletonPass : public FunctionPass {
  static char ID;
  SkeletonPass() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    // errs() << "I saw a function called " << F.getName() << "!\n";
    GVN gvn(F);
    gvn.run();
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
