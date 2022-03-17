// RUN: clang -Xclang -load -Xclang ../llvm-pass-skeleton/build/skeleton/libSkeletonPass.so %s | FileCheck %s
int main() {
  int a = 0;
  int b = 1;
  int c = a + b;
  for (int i = 0; i < 128; i++) {
    c += 2;
// CHECK: add nsw i32 %{{[0-9]+}}, 2
// CHECK-NOT: add nsw i32
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
    int d = a + b;
  }
  return 0;
}
