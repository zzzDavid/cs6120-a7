// RUN: clang -Xclang -load -Xclang ../llvm-pass-skeleton/build/skeleton/libSkeletonPass.so %s | FileCheck %s
int main() {
  int a = 0;
  int b = 1;
  int c = a + b;
  int max = 128;
  while(c < max) {
    c += 2;
// CHECK: store i32 %9, i32* %6
// CHECK-NOT: add
    int d = a + b;
  }
  return 0;
}
