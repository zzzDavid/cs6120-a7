int main() {
  int a = 1;
  int b = 1;
  int c = a + b;
  int d = a + b;
  int e = d;
  if (c > 1) {
    a = 1;
    b = 2;
  } else {
    a = 2;
    b = 1;
  }
  a += 1;
  return 0;
}
