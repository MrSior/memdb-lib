//
// Created by Семён on 17.11.2024.
//

#include "Compiler.h"

void Compiler::MoveItr(int off) {
    if (off > 0 && input_.end() - curLexemeItr_ - 1 < off) {
        curLexemeItr_ = input_.end() - 1;
    } else if (off < 0 && curLexemeItr_ - input_.begin() < abs(off)) {
        curLexemeItr_ = input_.begin();
    } else {
        curLexemeItr_ += off;
    }
}

Runtime Compiler::compile(const std::vector<Lexeme>& lexemes) {
    input_ = lexemes;
    curLexemeItr_ = input_.begin();

    Runtime runtime;
    Query(runtime);
    return runtime;
}

void Compiler::Query(Runtime& queries) {
    if (curLexemeItr_->type != ELexemeType::Keyword) {
        throw CompileException(*curLexemeItr_, "Query should start from keyword");
    }

    if (curLexemeItr_->str == "create") {
        ReadLexeme();
        if (curLexemeItr_->str == "table") {
            ReadLexeme();


        } else if (curLexemeItr_->str == "index") {
            // index logic
        } else {
            throw CompileException(*curLexemeItr_, "Unknown create (expected table or index)");
        }
    } else if (curLexemeItr_->str == "insert") {

    } else {
        throw CompileException(*curLexemeItr_, "Unknown query type");
    }
}


