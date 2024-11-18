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

    runtime_ = Runtime();
    Query();
    return runtime_;
}

void Compiler::Query() {
    if (curLexemeItr_->type != ELexemeType::Keyword) {
        throw CompileException(*curLexemeItr_, "Query should start from keyword");
    }

    if (curLexemeItr_->str == "create") {
        ReadLexeme();
        if (curLexemeItr_->str == "table") {
            ReadLexeme();
            Table();

            ReadLexeme();

            if (curLexemeItr_->type != ELexemeType::RoundBrackOp) {
                throw CompileException(*curLexemeItr_, "Expected \'(\'");
            }
            ReadLexeme();

            runtime_.putQuery(std::make_shared<QCreateTable>(Arguments()));
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

void Compiler::Table() {

}

std::vector<Column> Compiler::Arguments() {
    std::vector<Column> columns;
    while (curLexemeItr_->type != ELexemeType::RoundBrackCl) {
        if (!columns.empty()) {
            ReadLexeme();
        }

        Column col;
        if (curLexemeItr_->type == ELexemeType::CurlyBrackOp) {
            ReadLexeme();

            do {
                if (!col.attributes.empty()) {
                    ReadLexeme();
                }
                if (curLexemeItr_->type != ELexemeType::Keyword &&
                    !g_StrToAttrType.contains(curLexemeItr_->str)) {
                    throw CompileException(*curLexemeItr_, "expected column attribute");
                }

                col.attributes.push_back(g_StrToAttrType[curLexemeItr_->str]);
                ReadLexeme();
                if (LexemeDataToStr(*curLexemeItr_) != "," && curLexemeItr_->type != ELexemeType::CurlyBrackCl) {
                    throw CompileException(*curLexemeItr_, "expected \',\' as separator");
                }
            } while (curLexemeItr_->type != ELexemeType::CurlyBrackCl);

            ReadLexeme();
        }

        if (curLexemeItr_->type != ELexemeType::Identifier) {
            throw CompileException(*curLexemeItr_, "expected column name");
        }

        col.name = curLexemeItr_->str;

        ReadLexeme();
        if (LexemeDataToStr(*curLexemeItr_) != ":") {
            throw CompileException(*curLexemeItr_, "expected \':\'");
        }
        ReadLexeme();

        if (curLexemeItr_->type != ELexemeType::Keyword &&
            !g_StrToDbType.contains(curLexemeItr_->str)) {
            throw CompileException(*curLexemeItr_, "expected column value type");
        }
        col.type = g_StrToDbType[curLexemeItr_->str];

        ReadLexeme();

        if (col.type == EDbType::STRING || col.type == EDbType::BYTES) {
            if (curLexemeItr_->type != ELexemeType::BoxBrackOp) {
                throw CompileException(*curLexemeItr_, "expected \'[\'");
            }
            ReadLexeme();

            if (curLexemeItr_->type != ELexemeType::LiteralNum64) {
                throw CompileException(*curLexemeItr_, "expected size of array-like type");
            }

            col.size = curLexemeItr_->i64;

            ReadLexeme();
            if (curLexemeItr_->type != ELexemeType::BoxBrackCl) {
                throw CompileException(*curLexemeItr_, "expected \']\'");
            }

            ReadLexeme();
        }

        if (LexemeDataToStr(*curLexemeItr_) == "=") {
            ReadLexeme();
            switch (curLexemeItr_->type) {
                case ELexemeType::LiteralBool:
                    col.defaultValue = (bool)curLexemeItr_->i64;
                    break;
                case ELexemeType::LiteralNum64:
                    col.defaultValue = (int32_t)curLexemeItr_->i64;
                    break;
                case ELexemeType::LiteralBytes:
                case ELexemeType::LiteralStr:
                    if (curLexemeItr_->str.length() != col.size) {
                        throw CompileException(*curLexemeItr_, "incorrect default value");
                    }
                    col.defaultValue = curLexemeItr_->str;
                    break;
                default:
                    throw CompileException(*curLexemeItr_, "expected default value");
            }
            col.isHasDefault = true;
            ReadLexeme();
        }

        if (curLexemeItr_->type != ELexemeType::RoundBrackCl && LexemeDataToStr(*curLexemeItr_) != ",") {
            throw CompileException(*curLexemeItr_, "expected \',\' as separator");
        }
        columns.push_back(col);
    }

    return columns;
}

