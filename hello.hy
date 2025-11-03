int main() {
    int n = 4;
    int fac = 1;

    int i = 1;
    while (i <= n) {
        fac = fac * i;
        i = i + 1;
    }

    return fac(2, 5);
}
