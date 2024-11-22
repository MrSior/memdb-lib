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

Table::cell_t OperationNode::getResult(const THeader& header, const Table::row_t& row) {
    if (lexeme.type == ELexemeType::Operator) {
        auto leftRes = left->getResult(header, row);
        if (right == nullptr) {
            if (LexemeDataToStr(lexeme) == "-") {
                if (std::holds_alternative<int32_t>(leftRes)) {
                    return -(std::get<int32_t>(leftRes));
                } else {
                    throw RuntimeException("invalid operand type for unary \'-\'");
                }
            }
            if (LexemeDataToStr(lexeme) == "!") {
                if (std::holds_alternative<bool>(leftRes)) {
                    return !(std::get<bool>(leftRes));
                } else {
                    throw RuntimeException("invalid operand type for \'!\'");
                }
            }
            if (LexemeDataToStr(lexeme) == "#") {
                if (std::holds_alternative<bool>(leftRes)) {
                    return (int32_t)(std::get<std::string>(leftRes)).length();
                } else if (std::holds_alternative<bytes>(leftRes)) {
                    return static_cast<int32_t>((std::get<bytes>(leftRes).size() - 2) / 2);
                }else {
                    throw RuntimeException("invalid operand type for \'#\'");
                }
            }
        } else {
            auto rightRes = right->getResult(header, row);

            if (leftRes.index() != rightRes.index()) {
                throw RuntimeException("{expression evaluation} two operands of different types");
            }

            if (LexemeDataToStr(lexeme) == "+") {

            }
        }
    } else {
        if (lexeme.type == ELexemeType::Identifier) {
            for (int idx = 0; idx < header.columns.size(); ++idx) {
                const auto& col = header.columns[idx];
                if (col.name == lexeme.str) {
                    return row[idx];
                }
            }
        } else {
            switch (lexeme.type) {
                case ELexemeType::LiteralNum64:
                    return Table::cell_t{(int32_t) lexeme.i64};
                case ELexemeType::LiteralBool:
                    return Table::cell_t{(bool) lexeme.i64};
                case ELexemeType::LiteralBytes:
                    return Table::cell_t{bytes(lexeme.str.begin(), lexeme.str.end())};
                case ELexemeType::LiteralStr:
                    return Table::cell_t{lexeme.str};
                default:
                    throw RuntimeException("{expression evaluation} invalid list type in expression tree");
            }
        }
    }
}

void QSelect::exec() {

}
