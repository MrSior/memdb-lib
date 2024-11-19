//
// Created by Семён on 17.11.2024.
//

#include "Queries.h"

void QTable::exec() {

}

void QCreateTable::exec() {

}

void QInsert::exec() {

}

Table::cell_t OperationNode::getResult() {
    if (lexeme.type == ELexemeType::Operator) {
        auto leftRes = left->getResult();
        auto rightRes = right->getResult();

        if (leftRes.index() != rightRes.index()) {
            throw RuntimeException("{expression evaluation} two operands of different types");
        }

        if (LexemeDataToStr(lexeme) == "+") {

        }

    } else {
        switch (lexeme.type) {
            case ELexemeType::LiteralNum64:
                return Table::cell_t {(int32_t)lexeme.i64};
            case ELexemeType::LiteralBool:
                return Table::cell_t {(bool)lexeme.i64};
            case ELexemeType::LiteralBytes:
            case ELexemeType::LiteralStr:
                return Table::cell_t {lexeme.str};
            default:
                throw RuntimeException("{expression evaluation} invalid list type in expression tree");
        }
    }
}

void QSelect::exec() {

}
