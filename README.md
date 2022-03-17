# CS6120 Advanced Compiler A7: LLVM

This assignment is about how to develop a compiler pass in LLVM. 
`llvm-pass-skeleton` is a pass prototype that has the boiler-plate code to implement an LLVM pass.

## How to write an LLVM pass?
### Use clang to emit LLVM IR
```
$ clang -emit-llvm -S -o - main.cc
```
- `-S`: output LLVM assembly instead of binary
- `-o - `: output to stdio


### Build the Pass
```
cd llvm-pass-skeleton
mkdir build && cd build
cmake ..
make
```

### Run the Pass
Compile the program with clang and run the pass
```
clang -Xclang -load -Xclang ./llvm-pass-skeleton/build/skeleton/libSkeletonPass.so your_file.cc
```
There will be an `a.out` executable in the current directory. 

## LLVM Pass: Global Value Numbering (GVN)
I implemented a Common Sub-expression Elimination (CSE) pass using GVN.

## Testing
I developed my own test suite, and use LLVM's [lit](https://llvm.org/docs/CommandGuide/lit.html) and [FileCheck](https://llvm.org/docs/CommandGuide/FileCheck.html) as testing tools. To run the test suite:
```sh
lit tests
```

