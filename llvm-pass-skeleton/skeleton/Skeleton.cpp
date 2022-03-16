#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

class GVN {
public:
  // Constructor
  GVN(Function &F) : domTree(DominatorTree(F)) { this->Func = &F; }

  void run();
  void runOnBlock(BasicBlock &b);
  bool isValueEqual(const Value &v1, const Value &v2);

private:
  Function *Func;
  DominatorTree domTree;
  // env maps value to value number
  std::map<Value *, unsigned> env;
  // expression table [(opcode, [operand value number])]
  std::vector<std::pair<unsigned, std::vector<unsigned>>> table;
};

/* isValueEqual: Check if two values are loaded from the same pointer.
 * This is a helper function to check whether two expressions are
 * equivalent.
 * e.g.
 * %2 = add nsw i32 %0, %1
 * %5 = add nsw i32 %3, %4
 * If %0, %3 are loaded from the same pointer
 * and %1, %4 are loaded from the same pointer
 * Then these two add exprs are equivalent
 */
bool GVN::isValueEqual(const Value &v1, const Value &v2) {
  bool equal = false;
  if (auto *loadOp1 = dyn_cast<LoadInst>(&v1)) {
    if (auto *loadOp2 = dyn_cast<LoadInst>(&v2)) {
      equal = loadOp1->getOperand(0) == loadOp2->getOperand(0);
    }
  }
  return equal;
}

void GVN::runOnBlock(BasicBlock &B) {
  for (auto &I : B) {
    unsigned opcode = I.getOpcode();
    if (opcode == 33) {
      // skip store instruction since it's not an assignment
      continue;
    }
    // get value number for each operand
    std::vector<unsigned> operand_value_nums;
    for (unsigned i = 0; i < I.getNumOperands(); i++) {
      auto operand = I.getOperand(i);
      auto pos = env.find(operand);
      if (pos != env.end()) {
        // if operand in the env
        unsigned value_num = env[operand];
        operand_value_nums.push_back(value_num);
      }
    }
    auto value_tuple = std::make_pair(opcode, operand_value_nums);
    auto it = std::find(table.begin(), table.end(), value_tuple);
    bool computed = it != table.end();
    if (computed) {
      // add new mapping
      unsigned index = std::distance(table.begin(), it);
      env.insert(std::pair<Value *, unsigned>(&I, index));
    } else {
      unsigned index = table.size();
      table.push_back(value_tuple);
      env.insert(std::pair<Value *, unsigned>(&I, index));
    }
  }
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
