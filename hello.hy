
int main() {
    int i = 0;

    while (i < 10) {
        if (i % 2 == 1) {
            print_int(i);
        } else {
            print_int(0);
        }

        i = i + 1;
    }

    return 69;
}
