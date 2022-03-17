// RUN: clang -Xclang -load -Xclang ../llvm-pass-skeleton/build/skeleton/libSkeletonPass.so %s | FileCheck %s
int main() {
    int a = 1;
    int b = 1;
    int c = a + b;
    int d = a + c;
    int f = b + c;
    f = f + 2;
// CHECK: add nsw i32 %{{[0-9]+}}, 2
// CHECK-NOT: add nsw
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
    int r1 = a + b;
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
// CHECK-NOT: add nsw
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
    int r2 = a + c;
// CHECK: store i32 %{{[0-9]+}}, i32* %{{[0-9]+}}
// CHECK-NOT: add nsw
// CHECK: ret i32
    int r3 = b + c;
    return 0;
}
