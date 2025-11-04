int sub(int a, int b) {
    return a - b;
}

int fib(int n) {
    if (n <= 1) {
        return n;
    }

    return fib(n-1) + fib(n-2);
}

int main() {
    int a = 8;
    int b = a + 40;

    int res = fib(sub(b, a));

    print_int(res);

    return 0;
}
