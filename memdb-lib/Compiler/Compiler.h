#ifndef DATABASE_COMPILER_H
#define DATABASE_COMPILER_H

#include "Runtime.h"
#include "Parser/Lexeme.h"

#include <exception>
#include <string>

class CompileException : std::exception {
public:
    CompileException() = default;

    explicit CompileException(const Lexeme& curLexeme, std::string ctx) {
        str_ = "[CE] l:" + std::to_string(curLexeme.line) +
                " p:" + std::to_string(curLexeme.pos) + " " + std::move(ctx);
    }

    [[nodiscard]] const char* what() const noexcept override {
        return str_.c_str();
    }
private:
    std::string str_;
};

class Compiler {
public:
    Compiler() = default;

    Runtime compile(const std::vector<Lexeme>& lexemes);
private:
    Runtime runtime_;
    std::vector<Lexeme> input_;
    std::vector<Lexeme>::iterator curLexemeItr_;

    void ReadLexeme() { MoveItr(1); }
    void MoveItr(int off);


    void Query();
    void Table();
    std::vector<Column> Arguments();
};


#endif //DATABASE_COMPILER_H
