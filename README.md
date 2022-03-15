# CS6120 Advanced Compiler A7: LLVM

This assignment is about how to develop a compiler pass in LLVM. 
`llvm-pass-skeleton` is a pass prototype that has the boiler-plate code to implement an LLVM pass.


```
$ clang -emit-llvm -S -o - main.c
```
`-S`: assembly instead of binary
`-o - `: output to stdio

Compile the program with clang, and during compilation, run the pass
```
❯ clang -Xclang -load -Xclang ./llvm-pass-skeleton/build/skeleton/libSkeletonPass.so main.cc
```