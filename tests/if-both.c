// RUN: clang -Xclang -load -Xclang ../llvm-pass-skeleton/build/skeleton/libSkeletonPass.so %s | FileCheck %s
int main() {
  int a = 0;
  int b = 1;
  int c = a + b;
  if (c > 1) {
// CHECK: {{[0-9]+}}:
// CHECK-NOT: add nsw i32
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
    int d = a + b;
  } else {
// CHECK: {{[0-9]+}}:
// CHECK-NOT: add nsw i32
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
    int e = a + b;
  }
  c = c + 1;
  return 0;
}
