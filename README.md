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
I implemented a Common Sub-expression Elimination (CSE) pass using GVN following this [course blog](https://www.cs.cornell.edu/courses/cs6120/2019fa/blog/global-value-numbering/).

To run the GVN pass: 
```
clang -Xclang -load -Xclang ./llvm-pass-skeleton/build/skeleton/libSkeletonPass.so main.cc
```
The IR after the pass is printed in the console:
```
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  store i32 1, i32* %3, align 4
  %6 = load i32, i32* %2, align 4
  %7 = load i32, i32* %3, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = icmp sgt i32 %9, 1
  br i1 %10, label %11, label %12

11:                                               ; preds = %0
  store i32 %8, i32* %5, align 4
  br label %15

12:                                               ; preds = %0
  %13 = load i32, i32* %4, align 4
  %14 = add nsw i32 %13, 1
  store i32 %14, i32* %4, align 4
  br label %15

15:                                               ; preds = %12, %11
  ret i32 0
}
```
Notice that the `add` instruction generated from `int d = a + b` is eliminated, instead, there's only one store instruction `store i32 %8, i32* %5, align 4` to write the addition result in register to `d`'s pointer.


## Testing
I developed my own test suite, and use LLVM's [lit](https://llvm.org/docs/CommandGuide/lit.html) and [FileCheck](https://llvm.org/docs/CommandGuide/FileCheck.html) as testing tools. To run the test suite:
```sh
lit tests
```
In the tests, I use FileCheck's commands to check the common sub-expressions are indeed eliminated. For example, 
```c++
// CHECK: add nsw i32 %{{[0-9]+}}, 2
// CHECK-NOT: add nsw i32
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
```
The above checks states that there should not be an add instruction between `add nsw i32 %n, 2` and `store` instructions. The expected lit test output is: 
```
-- Testing: 6 tests, 6 workers --
PASS: test-suite :: for.c (1 of 6)
PASS: test-suite :: while.c (2 of 6)
PASS: test-suite :: basic.c (3 of 6)
PASS: test-suite :: if-both.c (4 of 6)
PASS: test-suite :: multiple.c (5 of 6)
PASS: test-suite :: if-single.c (6 of 6)

Testing Time: 0.63s
  Passed: 6
```
