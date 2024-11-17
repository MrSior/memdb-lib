#ifndef DATABASE_COMPILER_H
#define DATABASE_COMPILER_H

#include "Runtime.h"
#include "Parser/Lexeme.h"

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
    void MergeStacks(std::stack<std::shared_ptr<IQuery>> firstStack,
                     std::stack<std::shared_ptr<IQuery>> secondStack);


    std::stack<std::shared_ptr<IQuery>> Query();

    std::stack<std::shared_ptr<IQuery>> Table();
};


#endif //DATABASE_COMPILER_H
