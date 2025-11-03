if [ $# -ge 1 ]; then
    nasm -felf64 $1 -o out.o && {
        ld -o out out.o
        ./out
        echo "Exit code: $?"
        rm -rf out.o
        rm -rf out
    }
fi