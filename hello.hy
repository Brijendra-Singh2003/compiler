int main() {
    int a = 70;
    int b = 1;

    b -= a;

    print_int(a);
    print_int(b);

    if (b < 0) {
        return 1;
    }

    return 0;
}
