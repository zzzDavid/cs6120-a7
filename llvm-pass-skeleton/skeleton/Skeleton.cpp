#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
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
  void buildCopyInstr(Instruction *I, Value *src);
  void removeInsts();
  void printFunc();

private:
  Function *Func;
  DominatorTree domTree;
  // env maps value to value number
  std::map<Value *, unsigned> env;
  // expression table [(opcode, [operand value number])]
  std::vector<std::pair<unsigned, std::vector<unsigned>>> table;
  // instructions to erase
  SmallVector<Value *, 10> InstToRemove;
};

void GVN::printFunc() { llvm::outs() << *Func << "\n"; }

void GVN::removeInsts() {
  std::reverse(InstToRemove.begin(), InstToRemove.end());
  // llvm::outs() << "how many instructions to remove: " << InstToRemove.size()
  //              << "\n";
  for (auto *v : InstToRemove) {
    if (auto *I = dyn_cast<Instruction>(v)) {
      // llvm::outs() << "try to remove instr: " << *I << "\n";
      try {
        I->removeFromParent();
      } catch (const std::exception &e) {
        llvm::outs() << e.what();
      }
    }
  }
}

void GVN ::buildCopyInstr(Instruction *I, Value *src) {
  // Find the immediate storeInstruction that stores the
  // value of instruction I to a pointer. We then build
  // change the store op's operand to src.
  for (auto &U : I->uses()) {
    User *user = U.getUser();
    if (auto ii = dyn_cast<StoreInst>(user)) {
      user->setOperand(U.getOperandNo(), src);
      break;
    }
  }
}

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
    if (opcode == 33 or opcode == 32 or opcode == 1 or opcode == 2) {
      // skip store instruction since it's not an assignment
      // skip load instruction because it's handled in other instructions
      // skip br and ret
      continue;
    }
    llvm::outs() << "opcode: " << opcode << ", ";
    // get value number for each operand
    std::vector<unsigned> operand_value_nums;
    for (unsigned i = 0; i < I.getNumOperands(); i++) {
      auto operand = I.getOperand(i);
      if (auto loadOp = dyn_cast<LoadInst>(operand)) {
        auto realOperand = loadOp->getOperand(0);
        auto pos = env.find(realOperand);
        if (pos != env.end()) {
          // if operand in the env
          unsigned value_num = env[realOperand];
          operand_value_nums.push_back(value_num);
        }
      }
    }
    auto value_tuple = std::make_pair(opcode, operand_value_nums);
    auto it = std::find(table.begin(), table.end(), value_tuple);
    // alloca is always a new assignment
    bool computed = it != table.end() && opcode != 31;
    if (computed) {
      // add new mapping
      unsigned index = std::distance(table.begin(), it);
      env.insert(std::pair<Value *, unsigned>(&I, index));
      // llvm::outs() << "expression: " << I
      //              << " has been computed. Pointing it to index: " << index
      //              << "\n";
      // CSE logic
      Value *src_value = nullptr;
      for (auto it = env.begin(); it != env.end(); it++) {
        unsigned idx = it->second;
        if (idx == index) {
          buildCopyInstr(&I, it->first);
          InstToRemove.push_back(I.getOperand(0));
          InstToRemove.push_back(I.getOperand(1));
          InstToRemove.push_back(&I);
          break;
        }
      }
    } else {
      unsigned index = table.size();
      table.push_back(value_tuple);
      env.insert(std::pair<Value *, unsigned>(&I, index));
      // llvm::outs() << "expression: " << I
      //              << " is new, adding to table, got index: " << index <<
      //              "\n";
    }
  }
  // go to dominator tree child
  for (auto *child : domTree.getNode(&B)->children()) {
    runOnBlock(*(child->getBlock()));
  }

  // remove all values hashed during this function call
}

// entry function of GVN
void GVN::run() {
  runOnBlock(*domTree.getRoot());
  removeInsts();
  printFunc();
}

struct SkeletonPass : public FunctionPass {
  static char ID;
  SkeletonPass() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
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
