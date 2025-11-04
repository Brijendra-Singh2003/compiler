int sub(int a, int b) {
    return a - b;
}

int fac(int n) {
    if (n <= 1) {
        return n;
    }

    return fac(n-1) + fac(n-2);
}

int main() {
    int a = 8;
    //int b = a + 7;

    return fac(sub(b, a));
}
