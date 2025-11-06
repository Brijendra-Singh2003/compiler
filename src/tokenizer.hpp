#pragma once

#include "token.hpp"

#include <vector>
#include <unordered_map>

class Tokenizer {
public:
    Tokenizer(std::string& content) : content(content) {
        keywords = {
            { "int",    TokenType::INT },
            { "float",  TokenType::FLOAT },
            { "string",   TokenType::STRING },
            { "return", TokenType::RETURN },
            { "if",     TokenType::IF },
            { "else",   TokenType::ELSE },
            { "while",  TokenType::WHILE },
            { "for",    TokenType::FOR },
        };
    }

    std::vector<Token> tokenize() {
        tokens = std::vector<Token>();

        while (!isAtEnd()) {
            start = curr;
            char c = consume();

            switch (c) {
                // Single character tokens
            case '(': addToken(TokenType::LEFT_PAREN); break;
            case ')': addToken(TokenType::RIGHT_PAREN); break;
            case '{': addToken(TokenType::LEFT_BRACE); break;
            case '}': addToken(TokenType::RIGHT_BRACE); break;
            case ',': addToken(TokenType::COMMA); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case '+': addToken(TokenType::PLUS); break;
            case '-': addToken(TokenType::MINUS); break;
            case '*': addToken(TokenType::STAR); break;
            case '%': addToken(TokenType::PERCENTAGE); break;
            
            // One or two character tokens
            case '/':
                if (match('/')) {
                    while (!isAtEnd() && !match('\n')) {
                        advance();
                    }
                    ++line;
                } else if (match('*')) {
                    while (!isAtEnd() && !(match('*') && match('/'))) {
                        advance();
                    }
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                break;
            case '&':
                addToken(match('&') ? TokenType::AND_AND : TokenType::AND);
                break;
            case '|':
                addToken(match('|') ? TokenType::OR_OR : TokenType::OR);
                break;

                // Whitespace
            case '\n':
                line++;
            case ' ':
            case '\r':
            case '\t':
                break;

            default:
                if (std::isdigit(c)) {
                    readNumber();
                }
                else if (std::isalpha(c) || c == '_') {
                    readIdentifier();
                }
                else {
                    throw std::runtime_error("Unexpected character: " + std::string(1, c));
                }
                break;
            }
        }

        tokens.push_back(Token{
            .type = TokenType::EOF_TOKEN,
            .lexeme = "",
            .line = line,
        });

        return tokens;
    }

private:
    std::unordered_map<std::string, TokenType> keywords;
    std::vector<Token> tokens;
    std::string& content;
    size_t start = 0;
    size_t curr = 0;
    int line = 1;

    inline void readIdentifier() {
        while (isAlphanumeric(peek())) {
            advance();
        }

        std::string lexeme = content.substr(start, curr - start);
        TokenType token_type = TokenType::IDENTIFIER;

        auto it = keywords.find(lexeme);
        if (it != keywords.end()) {
            token_type = it->second;
        }

        tokens.push_back(Token{
            .type = token_type,
            .lexeme = lexeme,
            .line = line,
        });
    }

    inline void readNumber() {
        while (std::isdigit(peek())) {
            advance();
        }

        addToken(TokenType::INT_LIT);
    }

    inline char peek() {
        if (isAtEnd()) return '\0';
        return content[curr];
    }

    inline bool match(char expected) {
        if (peek() != expected) {
            return false;
        }

        curr++;
        return true;
    }

    inline char peekNext() {
        if (curr + 1 >= content.size()) return '\0';
        return content[curr + 1];
    }

    inline char consume() {
        char c = peek();
        advance();
        return c;
    }

    inline void advance() {
        ++curr;
    }

    inline bool isAtEnd() {
        return curr >= content.size();
    }

    inline bool isAlphanumeric(char c) {
        return std::isalpha(c) || std::isdigit(c) || c == '_';
    }

    inline void addToken(TokenType token_type) {
        tokens.push_back(Token{
            .type = token_type,
            .lexeme = content.substr(start, curr - start),
            .line = this->line,
        });
    }
};
