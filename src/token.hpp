#pragma once

#include <string>
#include <sstream>

enum TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, SEMICOLON, AND, OR,

    // unary Oprators
    BANG,

    // binary Operators
    PLUS, MINUS, STAR, SLASH, PERCENTAGE,
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,

    // Comparison operators  
    EQUAL, EQUAL_EQUAL, BANG_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,

    // logical operators
    AND_AND, OR_OR,
    AND_AND_EQUAL, OR_OR_EQUAL,

    // Keywords
    INT, FLOAT, STRING, RETURN, IF, ELSE, WHILE, FOR,

    // Literals
    PARAM, IDENTIFIER, INT_LIT,

    // misc
    FUNCTION_DECL, FUNCTION_CALL, STATEMENT_LIST, DECL_LIST, ARG_LIST,

    EOF_TOKEN,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    std::string toString() const {
        std::stringstream ss;
        ss << "Line " << line << ": ";
        
        switch(type) {
            case TokenType::LEFT_PAREN: ss << "LEFT_PAREN"; break;
            case TokenType::RIGHT_PAREN: ss << "RIGHT_PAREN"; break;
            case TokenType::LEFT_BRACE: ss << "LEFT_BRACE"; break;
            case TokenType::RIGHT_BRACE: ss << "RIGHT_BRACE"; break;
            case TokenType::COMMA: ss << "COMMA"; break;
            case TokenType::SEMICOLON: ss << "SEMICOLON"; break;
            case TokenType::PLUS: ss << "PLUS"; break;
            case TokenType::MINUS: ss << "MINUS"; break;
            case TokenType::STAR: ss << "STAR"; break;
            case TokenType::SLASH: ss << "SLASH"; break;
            case TokenType::AND: ss << "AND"; break;
            case TokenType::OR: ss << "OR"; break;
            case TokenType::EQUAL: ss << "EQUAL"; break;
            case TokenType::EQUAL_EQUAL: ss << "EQUAL_EQUAL"; break;
            case TokenType::BANG: ss << "BANG"; break;
            case TokenType::BANG_EQUAL: ss << "BANG_EQUAL"; break;
            case TokenType::LESS: ss << "LESS"; break;
            case TokenType::LESS_EQUAL: ss << "LESS_EQUAL"; break;
            case TokenType::GREATER: ss << "GREATER"; break;
            case TokenType::GREATER_EQUAL: ss << "GREATER_EQUAL"; break;
            case TokenType::AND_AND: ss << "AND_AND"; break;
            case TokenType::OR_OR: ss << "OR_OR"; break;
            case TokenType::INT: ss << "INT"; break;
            case TokenType::FLOAT: ss << "FLOAT"; break;
            case TokenType::STRING: ss << "STRING"; break;
            case TokenType::RETURN: ss << "RETURN"; break;
            case TokenType::IF: ss << "IF"; break;
            case TokenType::ELSE: ss << "ELSE"; break;
            case TokenType::WHILE: ss << "WHILE"; break;
            case TokenType::FOR: ss << "FOR"; break;
            case TokenType::IDENTIFIER: ss << "IDENTIFIER"; break;
            case TokenType::INT_LIT: ss << "INT_LIT"; break;
            case TokenType::EOF_TOKEN: ss << "EOF"; break;
            case TokenType::DECL_LIST: ss << "DECL_LIST"; break;
            case TokenType::PERCENTAGE: ss << "PERCENTAGE"; break;
        }
        
        ss << " '" << lexeme << "'";
        return ss.str();
    }
};
