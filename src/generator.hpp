#pragma once

#include "parser.hpp"
#include "util.hpp"

#include <iostream>
#include <sstream>
#include <memory>

class Generator {
public:
    Generator(const std::unique_ptr<TreeNode>& root) : root(root) {}

    std::string generateAsm64() {
        if (root == nullptr) {
            return "";
        }

        try {
            auto printfn_asm_code = readFile("./src/asm_lib/print_int.asm");

            asm_code << "global _start\n";
            asm_code << "_start:\n";
            asm_code << "   call _main\n";

            // Exit
            asm_code << "   ; Exit\n";
            asm_code << "   mov rdi, rax\n";
            asm_code << "   mov rax, 60\n";
            asm_code << "   syscall\n";
            
            generateDeclerationList(root);

            asm_code << printfn_asm_code;
            return asm_code.str();
        }
        catch(const std::runtime_error& e) {
            std::cerr << e.what() << '\n';
        }

        return "";
    }

private:
    const std::unique_ptr<TreeNode>& root;
    std::stringstream asm_code;
    int if_count = 0;

    void generateDeclerationList(const std::unique_ptr<TreeNode>& tree_node) {
        if (tree_node == nullptr) return;

        if (tree_node->token.type != TokenType::DECL_LIST) {
            throw std::runtime_error("DECL_LIST expected, got: " + tree_node->token.toString());
        }

        TreeNode* tmp = tree_node.get();
        while (tmp != nullptr) {
            if (tmp->left == nullptr) {
                throw std::runtime_error("Decleration expected. at line:" + std::to_string(tmp->token.line));
            }
            
            auto token = tmp->left->token;
            if (token.type == TokenType::FUNCTION_DECL) {
                generateFunction(tmp->left);
            }

            tmp = tmp->right.get();
        }
    }

    void generateFunction(const std::unique_ptr<TreeNode>& tree_node) {
        if (tree_node == nullptr) return;

        auto token = tree_node->token;
        if (token.type != TokenType::FUNCTION_DECL) return;

        const FuncNode* fn_node = dynamic_cast<FuncNode*>(tree_node.get());
        if (!fn_node) {
            throw std::runtime_error("Failed to convert to FuncNode.");
        }

        auto total_local_var_bytes = 16 * ((8 * fn_node->max_local_var_count + 15) / 16);
        auto fn_name = token.lexeme;

        asm_code << '\n' << fn_name << ":\n";
        asm_code << "   push rbp\n";
        asm_code << "   mov rbp, rsp\n";

        if (total_local_var_bytes) {
            asm_code << "   sub rsp, " << total_local_var_bytes << "\n";
        }

        generateStatementList(fn_node->right);
    }

    void generateStatementList(const std::unique_ptr<TreeNode>& tree_node) {
        if (tree_node == nullptr) {
            return;
        }

        if (tree_node->token.type != TokenType::STATEMENT_LIST) {
            throw std::runtime_error("Expected statement list, found '" + tree_node->token.toString() + "'. at line:" + std::to_string(tree_node->token.line));
        }

        generateStatement(tree_node->left);
        generateStatementList(tree_node->right);
    }

    void generateStatement(const std::unique_ptr<TreeNode>& tree_node) {
        if (tree_node == nullptr) return;
        const auto token_type = tree_node->token.type;

        if (token_type == TokenType::STATEMENT_LIST) {
            generateStatementList(tree_node);
            return;
        }

        if (token_type == TokenType::RETURN) {
            generateExpr(tree_node->right);

            asm_code << "   ; Return\n";
            asm_code << "   mov rsp, rbp\n";
            asm_code << "   pop rbp\n";
            asm_code << "   ret\n";
            return;
        }

        if (token_type == TokenType::IF) {
            auto if_node = dynamic_cast<IfNode*>(tree_node.get());
            auto l0 = "_L" + std::to_string(++if_count);
            auto l1 = "_L" + std::to_string(++if_count);
            
            generateExpr(if_node->condition);
            asm_code << "   cmp rax, 0\n";
            asm_code << "   jz " + l0 + "\n";
            
            generateStatementList(if_node->left);
            asm_code << "   jmp " + l1 + "\n";

            asm_code << l0 + ":\n";
            generateStatementList(if_node->right);

            asm_code << l1 + ":\n";
            return;
        }

        if (token_type == TokenType::WHILE) {
            auto l0 = "_L" + std::to_string(++if_count);
            auto l1 = "_L" + std::to_string(++if_count);

            asm_code << l0 << ":\n";
            generateExpr(tree_node->left);
            asm_code << "   jz " << l1 << '\n';

            generateStatementList(tree_node->right);
            asm_code << "   jmp " << l0 << '\n';
            asm_code << l1 << ":\n";
            return;
        }

        if (token_type == TokenType::INT) {
            if (tree_node->right == nullptr) return;

            if (tree_node->left == nullptr) {
                throw std::runtime_error("No variable name present in variable decleration. Line:" + std::to_string(tree_node->token.line));
            }

            auto id = tree_node->left->token.lexeme;
            generateExpr(tree_node->right);
            asm_code << "   mov qword [rbp - " << id << "], rax\n";
            return;
        }

        generateExpr(tree_node);
    }

    void generateExpr(const std::unique_ptr<TreeNode>& tree_node) {
        if (tree_node == nullptr) return;

        const auto token = tree_node->token;

        if (token.type == TokenType::INT_LIT) {
            asm_code << "   mov rax, " << token.lexeme << "\n";
            return;
        }

        if (token.type == TokenType::IDENTIFIER) {
            asm_code << "   mov rax, qword [rbp - " << token.lexeme << "]" << "\n";
            return;
        }

        if (token.type == TokenType::FUNCTION_CALL) {
            TreeNode* tmp = tree_node->left.get();
            int total_param_bytes = 0;

            while (tmp != nullptr) {
                if (tmp->token.type != TokenType::ARG_LIST) {
                    throw std::runtime_error("Arg expected in function call, got: " + tmp->token.toString());
                }

                generateExpr(tmp->right);
                asm_code << "   push rax\n";

                tmp = tmp->left.get();
                total_param_bytes += 8;
            }

            asm_code << "   call _" << token.lexeme << '\n';
            if (total_param_bytes > 0) {
                asm_code << "   add rsp, " << total_param_bytes << '\n';
            }

            return;
        }

        if (token.type == TokenType::EQUAL) {
            if (tree_node->left == nullptr) {
                throw std::runtime_error("Identifier expected before '='. at line:" + std::to_string(token.line));
            }

            auto var_id = tree_node->left->token.lexeme;

            generateExpr(tree_node->right);
            asm_code << "   mov qword [rbp - " << var_id << "], rax\n";
            return;
        }

        generateExpr(tree_node->left);
        asm_code << "   push rax\n";

        generateExpr(tree_node->right);
        asm_code << "   pop rbx\n";

        switch (token.type) {
        case TokenType::PLUS:
            asm_code << "   add rax, rbx\n";
            break;
        case TokenType::MINUS:
            asm_code << "   sub rbx, rax\n";
            asm_code << "   mov rax, rbx\n";
            break;
        case TokenType::STAR:
            asm_code << "   imul rax, rbx\n";
            break;
        case TokenType::SLASH:
            asm_code << "   mov rcx, rax\n";  // divisor in ecx
            asm_code << "   mov rax, rbx\n";  // dividend in eax
            asm_code << "   cdq\n";           // sign extend eax into edx:eax
            asm_code << "   idiv rcx\n";      // result in eax
            break;
        case TokenType::LESS:
            asm_code << "   cmp rbx, rax\n";
            asm_code << "   setl al\n";
            asm_code << "   and rax, 1\n";
            break;
        case TokenType::LESS_EQUAL:
            asm_code << "   cmp rbx, rax\n";
            asm_code << "   setle al\n";
            asm_code << "   and rax, 1\n";
            break;
        case TokenType::EQUAL_EQUAL:
            asm_code << "   cmp rbx, rax\n";
            asm_code << "   sete al\n";
            asm_code << "   and rax, 1\n";
            break;
        case TokenType::GREATER:
            asm_code << "   cmp rbx, rax\n";
            asm_code << "   setg al\n";
            asm_code << "   and rax, 1\n";
            break;
        case TokenType::GREATER_EQUAL:
            asm_code << "   cmp rbx, rax\n";
            asm_code << "   setge al\n";
            asm_code << "   and rax, 1\n";
            break;
        case TokenType::AND:
            asm_code << "   and rax, rbx\n";
            break;
        case TokenType::OR:
            asm_code << "   or rax, rbx\n";
            break;
        case TokenType::AND_AND:
            asm_code << "   cmovnz rbx, rax\n";
            asm_code << "   mov rax, rbx\n";
            break;
            case TokenType::OR_OR:
            asm_code << "   cmovz rbx, rax\n";
            asm_code << "   mov rax, rbx\n";
            break;
        case TokenType::BANG:
            asm_code << "   not rax\n";
            asm_code << "   and rax, 1\n";
            break;
        default:
            throw std::runtime_error("Invalid token '" + token.lexeme + "' at line:" + std::to_string(token.line));
        }
    }
};
