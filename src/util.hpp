#pragma once

#include "parser.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <queue>

std::string readFile(std::string filePath) {
    std::stringstream ss;

    if (auto fin = std::ifstream(filePath)) {
        ss << fin.rdbuf();
    } else {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    return ss.str();
}

void printTreeLevelOrder(std::unique_ptr<TreeNode>& root) {
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

void printTreePreOrder(const std::unique_ptr<TreeNode>& root) {
    if (root == nullptr) {
        return;
    }

    std::clog << root->token.lexeme << ' ';
    printTreePreOrder(root->left);
    printTreePreOrder(root->right);
}
