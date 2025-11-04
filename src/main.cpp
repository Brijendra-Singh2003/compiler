#include "tokenizer.hpp"
#include "parser.hpp"
#include "generator.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>

void printTree(std::unique_ptr<TreeNode>& root) {
    if (root == nullptr) {
        return;
    }

    std::queue<TreeNode*> q;
    q.push(root.get());

    while (!q.empty()) {
        int levelSize = q.size();

        for (int i = 0; i < levelSize; i++) {
            TreeNode* current = q.front();
            q.pop();
            
            std::clog << current->token.lexeme << '\t';
            
            if (current->left != nullptr) {
                q.push(current->left.get());
            }
            if (current->right != nullptr) {
                q.push(current->right.get());
            }
        }
        
        std::clog << std::endl;
    }
}

void printTreeInOrder(const std::unique_ptr<TreeNode>& root) {
    if (root == nullptr) {
        return;
    }

    printTreeInOrder(root->left);
    std::clog << root->token.lexeme << ' ';
    printTreeInOrder(root->right);
}

void printTreePreOrder(const std::unique_ptr<TreeNode>& root) {
    if (root == nullptr) {
        return;
    }

    std::clog << root->token.lexeme << ' ';
    printTreePreOrder(root->left);
    printTreePreOrder(root->right);
}

std::string readFile(std::string filePath) {
    std::ifstream fin(filePath);

    if (!fin) {
        std::cerr << "Unable to open file.\n";
        exit(EXIT_FAILURE);
    }

    std::stringstream ss;
    ss << fin.rdbuf();

    return ss.str();
}

std::unique_ptr<TreeNode> parseCode(std::vector<Token>& tokens) {
    Parser parser(tokens);
    try {
        return parser.parseProgram();
    }
    catch (std::runtime_error err) {
        std::cerr << err.what() << std::endl;
        return nullptr;
    }
    catch (std::logic_error err) {
        std::cerr << err.what() << std::endl;
        return nullptr;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Requires a file path in the arguments.\n";
        return EXIT_FAILURE;
    }

    std::string code = readFile(argv[1]);

    Tokenizer tokenizer(code);
    auto tokens = tokenizer.tokenize();

    for (auto token : tokens) {
        std::clog << token.toString() << std::endl;
    }

    auto tree_root = parseCode(tokens);
    if (tree_root == nullptr) {
        return EXIT_FAILURE;
    }

    std::clog << "Tree generated.\n";

    printTree(tree_root);
    std::clog << "\n\n";

    std::clog << tree_root->toString() << std::endl;

    printTreePreOrder(tree_root);
    std::clog << "\n";

    Generator generator(tree_root);
    std::string asm_code = generator.generateAsm64();

    if (auto fout = std::ofstream("./out.asm")) {
        fout << asm_code;
    } else {
        std::cerr << "Unable to open asm file.\n";
    }

    return 0;
}
