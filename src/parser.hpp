#pragma once

#include "tokenizer.hpp"

#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>
#include <optional>
#include <stdexcept>

class TreeNode {
public:
    Token token;
    std::unique_ptr<TreeNode> left, right;

    virtual std::string toString() {
        std::string str = "";

        if (left != nullptr) {
            str += left->toString() + " ";
        }

        str += token.lexeme;

        if (right != nullptr) {
            str += " " + right->toString();
        }

        return str;
    }

    TreeNode(Token token) : token(token), left(nullptr), right(nullptr) {}
    TreeNode(Token token, std::unique_ptr<TreeNode> left, std::unique_ptr<TreeNode> right)
        : token(token), left(std::move(left)), right(std::move(right)) {}
};

class FuncNode : public TreeNode {
public:
    int max_local_var_count = 0;

    FuncNode(Token token, std::unique_ptr<TreeNode> left = nullptr, std::unique_ptr<TreeNode> right = nullptr)
        : TreeNode(token, std::move(left), std::move(right)) {}

    virtual std::string toString() override {
        std::string str = "";

        if (left != nullptr) {
            str += left->toString() + " ";
        }

        str += token.lexeme + "_" + std::to_string(max_local_var_count);

        if (right != nullptr) {
            str += " " + right->toString();
        }

        return str;
    }
};

class IfNode : public TreeNode {
public:
    std::unique_ptr<TreeNode> condition;

    IfNode(Token token,
        std::unique_ptr<TreeNode> left = nullptr,
        std::unique_ptr<TreeNode> right = nullptr,
        std::unique_ptr<TreeNode> condition = nullptr
    ) : TreeNode(token, std::move(left), std::move(right)), condition(std::move(condition)) {}

    virtual std::string toString() override {
        std::string str = "";

        if (left != nullptr) {
            str += left->toString() + " ";
        }

        str += token.lexeme + "(" + condition->toString() + ")";

        if (right != nullptr) {
            str += " " + right->toString();
        }

        return str;
    }
};

class Parser {
public:
    Parser(std::vector<Token>& tokens) : tokens(tokens), curr(0) {}

    std::unique_ptr<TreeNode> parseProgram() {
        auto root = parseDeclarationList();
        if (!isAtEnd()) {
            throw std::runtime_error("Unexpected tokens after program end");
        }
        return root;
    }

    std::unique_ptr<TreeNode> parseDeclarationList() {
        if (isAtEnd()) return nullptr;

        Token token{
            .type = TokenType::DECL_LIST,
            .lexeme = "dec",
            .line = peek().line,
        };

        std::unique_ptr<TreeNode> left = parseDeclaration();
        std::unique_ptr<TreeNode> right = parseDeclarationList();

        return std::make_unique<TreeNode>(token, std::move(left), std::move(right));
    }

    std::unique_ptr<TreeNode> parseDeclaration() {
        if (!isTypeKeyword()) {
            throw std::runtime_error("Expected a keyword");
        }

        auto keyword = consume();

        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected identifier after keyword at line: " + std::to_string(keyword.line));
        }

        auto identifier = consume();
        auto left = std::make_unique<TreeNode>(identifier);

        // Function declaration
        if (match(TokenType::LEFT_PAREN)) {
            local_var_names.push_back(std::unordered_map<std::string, int>());

            Token fn_token{
                .type = TokenType::FUNCTION_DECL,
                .lexeme = "_" + identifier.lexeme,
                .line = keyword.line,
            };

            advance(); // consume '('
            auto params = parseParams();

            if (!match(TokenType::RIGHT_PAREN)) {
                throw std::runtime_error("Expected ')' after parameters");
            }
            advance(); // consume ')'
            
            this->max_local_vars_count = 0;
            auto block = parseBlock();

            auto fn_sign = std::make_unique<TreeNode>(keyword, std::move(left), std::move(params));

            auto fn_node = std::make_unique<FuncNode>(fn_token, std::move(fn_sign), std::move(block));
            fn_node->max_local_var_count = this->max_local_vars_count;
            
            local_var_names.pop_back();

            return fn_node;
        }

        if (match(TokenType::SEMICOLON)) {
            advance();

            return std::make_unique<TreeNode>(keyword, std::move(left), nullptr);
        }

        if (match(TokenType::EQUAL)) {
            advance(); // consume '='
            auto value = parseExpression();

            if (!match(TokenType::SEMICOLON)) {
                throw std::runtime_error("Expected ';' after variable declaration");
            }
            advance(); // consume ';'

            return std::make_unique<TreeNode>(keyword, std::move(left), std::move(value));
        }

        throw std::runtime_error("Expected a ';' or '=' after identifier at line:" + std::to_string(identifier.line));
    }

    std::unique_ptr<TreeNode> parseBlock() {
        if (!match(TokenType::LEFT_BRACE)) {
            throw std::runtime_error("Expected '{' at line: " + std::to_string(peek().line));
        }
        auto token = consume(); // consume '{'

        local_var_names.push_back(std::unordered_map<std::string, int>());
        auto statements = parseStatementList();
        local_vars_count -= local_var_names.back().size();
        local_var_names.pop_back();

        if (!match(TokenType::RIGHT_BRACE)) {
            throw std::runtime_error("Missing '}' for '{' at line: " + std::to_string(token.line));
        }
        advance(); // consume '}'

        return statements;
    }

    std::unique_ptr<TreeNode> parseStatementList() {
        if (match(TokenType::RIGHT_BRACE) || isAtEnd()) {
            return nullptr;
        }

        auto line = peek().line;
        auto left = parseStatement();
        auto right = parseStatementList();

        Token token{
            .type = ::STATEMENT_LIST,
            .lexeme = "stmt",
            .line = line
        };
        return std::make_unique<TreeNode>(token, std::move(left), std::move(right));
    }

    std::unique_ptr<TreeNode> parseStatement() {
        if (match(TokenType::RETURN)) {
            auto return_token = consume();
            auto expr = parseExpression();

            if (!match(TokenType::SEMICOLON)) {
                throw std::runtime_error("Expected ';' after return statement. Line:" + std::to_string(return_token.line));
            }
            advance(); // consume ';'

            return std::make_unique<TreeNode>(return_token, nullptr, std::move(expr));
        }

        if (match(TokenType::IF)) {
            auto if_token = consume();

            if (!match(TokenType::LEFT_PAREN)) {
                throw std::runtime_error("'(' expected after if. Line:" + std::to_string(if_token.line));
            }
            advance(); // consume '('

            auto condition = parseExpression();

            if (!match(TokenType::RIGHT_PAREN)) {
                throw std::runtime_error("missing ')' for '(' at line:" + std::to_string(if_token.line));
            }
            advance(); // consume ')'

            auto if_body = parseBlock();
            std::unique_ptr<TreeNode> else_body = nullptr;

            if (match(TokenType::ELSE)) {
                advance(); // consume else token

                else_body = parseBlock();
            }

            return std::make_unique<IfNode>(if_token, std::move(if_body), std::move(else_body), std::move(condition));
        }

        if (match(TokenType::WHILE)) {
            auto while_token = consume();

            if (!match(TokenType::LEFT_PAREN)) {
                throw std::runtime_error("Expected '(' after while. at line:" + std::to_string(while_token.line));
            }

            advance();
            auto condition = parseExpression();
            if (!match(TokenType::RIGHT_PAREN)) {
                throw std::runtime_error("Expected ')' after expression. at line:" + std::to_string(while_token.line));
            }
            advance();

            auto while_body = parseBlock();

            return std::make_unique<TreeNode>(while_token, std::move(condition), std::move(while_body));
        }

        if (isTypeKeyword()) {
            auto keyword = consume();

            if (!match(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected identifier after keyword at line: " + std::to_string(keyword.line));
            }

            auto identifier = consume();

            std::unique_ptr<TreeNode> init = nullptr;

            if (match(TokenType::EQUAL)) {
                advance(); // consume '='
                init = parseExpression();
            }

            if (!match(TokenType::SEMICOLON)) {
                throw std::runtime_error("Expected ';' after variable declaration at Line:" + std::to_string(identifier.line));
            }
            advance(); // consume ';'

            auto var_name = identifier.lexeme;
            if (local_var_names.back().find(var_name) != local_var_names.back().end()) {
                throw std::runtime_error("Variable '" + var_name + "' is already decleared. line:" + std::to_string(identifier.line));
            }

            ++local_vars_count;
            local_var_names.back()[var_name] = local_vars_count;
            identifier.lexeme = std::to_string(local_vars_count * 8);
            max_local_vars_count = std::max(max_local_vars_count, local_vars_count);

            return std::make_unique<TreeNode>(
                keyword,
                std::make_unique<TreeNode>(identifier),
                std::move(init)
            );
        }

        if (match(TokenType::LEFT_BRACE)) {
            return parseBlock();
        }

        auto expr = parseExpression();

        if (!match(TokenType::SEMICOLON)) {
            throw std::runtime_error("Expected ';' after expression at line: " + std::to_string(peek().line));
        }
        advance(); // consume ';'

        return expr;
    }

    std::unique_ptr<TreeNode> parseParams() {
        if (match(TokenType::RIGHT_PAREN)) {
            return nullptr;
        }

        int param_pos = 1;
        auto left = parseParam(++param_pos);

        while (match(TokenType::COMMA)) {
            auto comma = consume(); // consume ','
            auto right = parseParam(++param_pos);
            left = std::make_unique<TreeNode>(comma, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseParam(int pos) {
        if (!isTypeKeyword()) {
            throw std::runtime_error("Expected type keyword in parameter");
        }
        auto type = consume();

        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected parameter name at Line:" + std::to_string(type.line));
        }
        auto name = consume();
        local_var_names.back()[name.lexeme] = -pos;

        return std::make_unique<TreeNode>(type, std::make_unique<TreeNode>(name), nullptr);
    }

    // Expression parsing with proper precedence (highest to lowest)
    std::unique_ptr<TreeNode> parseExpression() {
        return parseAssignment();
    }

    std::unique_ptr<TreeNode> parseAssignment() {
        auto left = parseLogicalOr();

        if (match(TokenType::EQUAL)) {
            auto op = consume();
            auto right = parseAssignment();
            return std::make_unique<TreeNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseLogicalOr() {
        auto left = parseLogicalAnd();

        while (match(TokenType::OR_OR)) {
            auto op = consume();
            auto right = parseLogicalAnd();
            left = std::make_unique<TreeNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseLogicalAnd() {
        auto left = parseComparison();

        while (match(TokenType::AND_AND)) {
            auto op = consume();
            auto right = parseComparison();
            left = std::make_unique<TreeNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseComparison() {
        auto left = parseTerm();

        while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
            match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) ||
            match(TokenType::EQUAL_EQUAL)) {
            auto op = consume();
            auto right = parseTerm();
            left = std::make_unique<TreeNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseTerm() {
        auto left = parseFactor();

        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            auto op = consume();
            auto right = parseFactor();
            left = std::make_unique<TreeNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseFactor() {
        auto left = parseUnary();

        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENTAGE)) {
            auto op = consume();
            auto right = parseUnary();
            left = std::make_unique<TreeNode>(op, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<TreeNode> parseUnary() {
        if (match(TokenType::MINUS) || match(TokenType::BANG)) {
            auto op = consume();
            auto operand = parseUnary();
            return std::make_unique<TreeNode>(op, nullptr, std::move(operand));
        }

        return parsePrimary();
    }

    std::unique_ptr<TreeNode> parsePrimary() {
        if (isAtEnd()) {
            throw std::runtime_error("Unexpected end of input");
        }

        Token token = peek();

        if (match(TokenType::IDENTIFIER)) {
            advance();

            // check if it's a function call
            if (match(TokenType::LEFT_PAREN)) {
                advance();

                Token call_token{
                    .type = TokenType::FUNCTION_CALL,
                    .lexeme = token.lexeme,
                    .line = token.line
                };

                Token arg_list_token {
                    .type = TokenType::ARG_LIST,
                    .lexeme = "arg",
                    .line = call_token.line
                };

                std::unique_ptr<TreeNode> left = nullptr;

                while (!match(TokenType::RIGHT_PAREN)) {
                    auto arg = parseExpression();
                    left = std::make_unique<TreeNode>(arg_list_token, std::move(left), std::move(arg));

                    if (!match(TokenType::COMMA)) break;
                    advance();
                };

                if (!match(TokenType::RIGHT_PAREN)) {
                    throw std::runtime_error("Missing ')' for function call. at line:" + std::to_string(call_token.line));
                }
                advance();

                return std::make_unique<TreeNode>(call_token, std::move(left), nullptr);
            }

            if (auto val = tryGetVarId(token.lexeme)) {
                token.lexeme = val.value();
                return std::make_unique<TreeNode>(token);
            }

            throw std::runtime_error("Variable '" + token.lexeme + "' not decleared in this scope. Line:" + std::to_string(token.line));
        }

        switch (token.type) {
        case TokenType::INT_LIT:
            advance();
            return std::make_unique<TreeNode>(token);

        case TokenType::LEFT_PAREN: {
            advance(); // consume '('
            auto expr = parseExpression();

            if (!match(TokenType::RIGHT_PAREN)) {
                throw std::runtime_error("Expected ')'");
            }
            advance(); // consume ')'
            return expr;
        }

        default:
            throw std::runtime_error("Unexpected token in expression: " + token.lexeme + " at line: " + std::to_string(token.line));
        }
    }

private:
    std::vector<Token>& tokens;
    std::vector<std::unordered_map<std::string, int>> local_var_names;
    int max_local_vars_count = 0;
    int local_vars_count = 0;
    size_t curr;

    inline bool isAtEnd() {
        return curr >= tokens.size() || tokens[curr].type == TokenType::EOF_TOKEN;
    }

    inline Token peek() {
        if (isAtEnd()) {
            return Token{ TokenType::EOF_TOKEN, "", 0 };
        }
        return tokens[curr];
    }

    inline Token consume() {
        if (isAtEnd()) {
            throw std::runtime_error("Unexpected end of input");
        }
        return tokens[curr++];
    }

    inline void advance() {
        if (!isAtEnd()) {
            ++curr;
        }
    }

    inline bool match(TokenType token_type) {
        return !isAtEnd() && peek().type == token_type;
    }

    inline bool matchNext(TokenType token_type) {
        if (curr + 1 >= tokens.size()) return false;
        return tokens[curr + 1].type == token_type;
    }

    inline bool isTypeKeyword() {
        if (isAtEnd()) return false;
        return match(TokenType::STRING) || match(TokenType::INT) || match(TokenType::FLOAT);
    }

    std::optional<std::string> tryGetVarId(std::string var_name) {
        for (int i = local_var_names.size() - 1; i >= 0; --i) {
            if (local_var_names[i].count(var_name)) {
                return std::to_string(local_var_names[i][var_name] * 8);
            }
        }

        return std::nullopt;
    }
};