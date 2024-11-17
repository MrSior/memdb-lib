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


}


