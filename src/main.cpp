#include "tokenizer.hpp"
#include "parser.hpp"
#include "generator.hpp"
#include "util.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Requires a file path in the arguments.\n";
        return EXIT_FAILURE;
    }

    try {
        std::string code = readFile(argv[1]);


        Tokenizer tokenizer(code);
        auto tokens = tokenizer.tokenize();

        for (auto token : tokens) {
            std::clog << token.toString() << std::endl;
        }


        Parser parser(tokens);
        auto tree_root = parser.parseProgram();

        if (tree_root == nullptr) {
            return EXIT_FAILURE;
        }
        std::clog << "Tree generated.\n";

        printTreeLevelOrder(tree_root);
        std::clog << "\n\n";
        std::clog << tree_root->toString() << std::endl;
        printTreePreOrder(tree_root);
        std::clog << "\n";


        Generator generator(tree_root);
        std::string asm_code = generator.generateAsm64();

        if (auto fout = std::ofstream("./out.asm")) {
            fout << asm_code;
        }
        else {
            std::cerr << "Unable to open out.asm file.\n";
        }
    }
    catch (std::runtime_error err) {
        std::cerr << err.what() << std::endl;
    }
    catch (std::logic_error err) {
        std::cerr << err.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
