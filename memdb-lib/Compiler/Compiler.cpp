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

    if (curLexemeItr_->type != ELexemeType::ProgramEnd) {
        throw CompileException(*curLexemeItr_, "unexpected input after query");
    }

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

            if (curLexemeItr_->type != ELexemeType::Identifier) {
                throw CompileException(*curLexemeItr_, "expected table name");
            }
            std::string tName = curLexemeItr_->str;
            ReadLexeme();

            if (curLexemeItr_->type != ELexemeType::RoundBrackOp) {
                throw CompileException(*curLexemeItr_, "Expected \'(\'");
            }
            ReadLexeme();

            runtime_.putQuery(std::make_shared<QCreateTable>(THeader(tName, Arguments())));
        } else if (curLexemeItr_->str == "index") {
            // index logic
        } else {
            throw CompileException(*curLexemeItr_, "Unknown create (expected table or index)");
        }
    } else if (curLexemeItr_->str == "insert") {
        ReadLexeme();

        if (curLexemeItr_->type != ELexemeType::RoundBrackOp) {
            throw CompileException(*curLexemeItr_, "expected \'(\')");
        }
        ReadLexeme();

        auto values = Values();

        if (curLexemeItr_->type != ELexemeType::Keyword && LexemeDataToStr(*curLexemeItr_) != "to") {
            throw CompileException(*curLexemeItr_, "expected keyword \'to\'");
        }
        ReadLexeme();

        if (curLexemeItr_->type != ELexemeType::Identifier) {
            throw CompileException(*curLexemeItr_, "expected table name");
        }

//        auto tName = curLexemeItr_->str;
        runtime_.putQuery(std::make_shared<QTable>(curLexemeItr_->str));
        ReadLexeme();

        runtime_.putQuery(std::make_shared<QInsert>(values));
    } else if (curLexemeItr_->str == "select") {
        ReadLexeme();

        auto columns = Columns();
        if (curLexemeItr_->str != "from") {
            throw CompileException(*curLexemeItr_, "expected keyword \'from\'");
        }
        ReadLexeme();
        Table();

        if (curLexemeItr_->str != "where") {
            throw CompileException(*curLexemeItr_, "expected keyword \'where\'");
        }

        ReadLexeme();
        auto condition = Expression();

        runtime_.putQuery(std::make_shared<QSelect>(columns, condition));
    } else if (curLexemeItr_->str == "update") {
        ReadLexeme();

        if (curLexemeItr_->type != ELexemeType::Identifier) {
            throw CompileException(*curLexemeItr_, "expected table name");
        }

        runtime_.putQuery(std::make_shared<QTable>(curLexemeItr_->str));
        ReadLexeme();

        if (LexemeDataToStr(*curLexemeItr_) != "set") {
            throw CompileException(*curLexemeItr_, R"(expected keyword 'set')");
        }
        ReadLexeme();

        auto assigns = Assignments();

        if (LexemeDataToStr(*curLexemeItr_) != "where") {
            throw CompileException(*curLexemeItr_, R"(expected keyword 'where')");
        }

        ReadLexeme();
        auto condition = Expression();

        runtime_.putQuery(std::make_shared<QUpdate>(assigns, condition));
    } else if (curLexemeItr_->str == "delete") {
        ReadLexeme();
        if (curLexemeItr_->type != ELexemeType::Identifier) {
            throw CompileException(*curLexemeItr_, "expected table name");
        }

        runtime_.putQuery(std::make_shared<QTable>(curLexemeItr_->str));
        ReadLexeme();

        if (LexemeDataToStr(*curLexemeItr_) != "where") {
            throw CompileException(*curLexemeItr_, R"(expected keyword 'where')");
        }

        ReadLexeme();
        auto condition = Expression();

        runtime_.putQuery(std::make_shared<QDelete>(condition));
    }
    else {
        throw CompileException(*curLexemeItr_, "Unknown query type");
    }
}

void Compiler::Table() {
    if (curLexemeItr_->type == ELexemeType::Identifier) {
        runtime_.putQuery(std::make_shared<QTable>(curLexemeItr_->str));
        ReadLexeme();

        // here goes join case parsing logic
        return;
    }
    if (curLexemeItr_->type == ELexemeType::Keyword &&
        LexemeDataToStr(*curLexemeItr_) == "select") {
        Query();
        return;
    }
    throw CompileException(*curLexemeItr_, "invalid table expression");
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
                    if ((curLexemeItr_->str.length() - 2) / 2 + ((curLexemeItr_->str.length() - 2) % 2) > col.size) {
                        throw CompileException(*curLexemeItr_, "incorrect default value");
                    }
                    col.defaultValue = bytes(curLexemeItr_->str.begin(), curLexemeItr_->str.end());
                    break;
                case ELexemeType::LiteralStr:
                    if (curLexemeItr_->str.length() > col.size) {
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

    ReadLexeme();
    return columns;
}

QInsert::queryData_t Compiler::Values() {
    QInsert::queryData_t values;

    auto readVal = [&]() -> QInsert::queryParam_t {
        QInsert::queryParam_t elem = {"", nullptr};
        if (curLexemeItr_->type == ELexemeType::RoundBrackCl ||
            LexemeDataToStr(*curLexemeItr_) == ",") {
            return elem;
        }

        if (curLexemeItr_->type == ELexemeType::Identifier) {
            elem.first = curLexemeItr_->str;

            ReadLexeme();
            if (LexemeDataToStr(*curLexemeItr_) != "=") {
                throw CompileException(*curLexemeItr_, "expected \'=\'");
            }
            ReadLexeme();
        }

        switch (curLexemeItr_->type) {
            case ELexemeType::LiteralNum64:
                elem.second = (int32_t)curLexemeItr_->i64;
                break;
            case ELexemeType::LiteralBool:
                elem.second = (bool)curLexemeItr_->i64;
                break;
            case ELexemeType::LiteralBytes:
                elem.second = bytes(curLexemeItr_->str.begin(), curLexemeItr_->str.end());
                break;
            case ELexemeType::LiteralStr:
                elem.second = curLexemeItr_->str;
                break;
            default:
                throw CompileException(*curLexemeItr_, "expected column value");
        }
        ReadLexeme();

        return elem;
    };

    while (true) {
        values.push_back(readVal());
        if (curLexemeItr_->type == ELexemeType::RoundBrackCl) {
            break;
        }
        if (LexemeDataToStr(*curLexemeItr_) == ",") {
            ReadLexeme();
            continue;
        }
        throw CompileException(*curLexemeItr_, R"(expected '(' or ',')");
    }

    ReadLexeme();
    return values;
}

std::vector<std::string> Compiler::Columns() {
    std::vector<std::string> cols;
    while (LexemeDataToStr(*curLexemeItr_) != "from" &&
           curLexemeItr_->type != ELexemeType::ProgramEnd) {
        if (curLexemeItr_->type != ELexemeType::Identifier) {
            throw CompileException(*curLexemeItr_, "expected column name");
        }
        cols.push_back(curLexemeItr_->str);
        ReadLexeme();
        if (LexemeDataToStr(*curLexemeItr_) == ".") {
            cols.back() += ".";
            ReadLexeme();
            if (curLexemeItr_->type != ELexemeType::Identifier) {
                throw CompileException(*curLexemeItr_, "invalid column name");
            }
            cols.back() += curLexemeItr_->str;
            ReadLexeme();
        }
        if (LexemeDataToStr(*curLexemeItr_) != "," && LexemeDataToStr(*curLexemeItr_) != "from") {
            throw CompileException(*curLexemeItr_, "expected \',\' separator");
        } else if (LexemeDataToStr(*curLexemeItr_) == ",") {
            ReadLexeme();
        }
    }
    return cols;
}

std::shared_ptr<OperationNode> Compiler::Expression(int level) {
    if (level < 8) {
        std::shared_ptr<OperationNode> res = Expression(level + 1);

        auto ops = exprLevelToOperations_[level];
        while (std::find(ops.begin(), ops.end(), curLexemeItr_->str) != ops.end()) {
            auto tmp = res;
            res = std::make_shared<OperationNode>(*curLexemeItr_);
            res->left = tmp;

            ReadLexeme();
            res->right = Expression(level + 1);;
        }

        return res;
    } else if (level == 8) {
        auto ops = exprLevelToOperations_[level];
        if (std::find(ops.begin(), ops.end(), curLexemeItr_->str) != ops.end()) {
            auto res = std::make_shared<OperationNode>(*curLexemeItr_);

            ReadLexeme();

            res->left = Expression(level + 1);
            return res;
        } else {
            return Expression(level + 1);
        }
    } else if (level == 9) {
        if (curLexemeItr_->type == ELexemeType::RoundBrackOp) {
            ReadLexeme();
            auto res = Expression(1);
            if (curLexemeItr_->type != ELexemeType::RoundBrackCl) {
                throw CompileException(*curLexemeItr_, "expected closing round bracket");
            }
            ReadLexeme();
            return res;
        } else {
            if (curLexemeItr_->type != ELexemeType::Identifier &&
                curLexemeItr_->type != ELexemeType::LiteralNum64 &&
                curLexemeItr_->type != ELexemeType::LiteralBool &&
                curLexemeItr_->type != ELexemeType::LiteralStr &&
                curLexemeItr_->type != ELexemeType::LiteralBytes) {
                throw CompileException(*curLexemeItr_, "expected operand");
            }
            auto res = std::make_shared<OperationNode>(*curLexemeItr_);
            ReadLexeme();
            return res;
        }
    }
    throw CompileException(*curLexemeItr_, "invalid level during parsing expression");
}

std::vector<QUpdate::assign_t> Compiler::Assignments() {
    std::vector<QUpdate::assign_t> assigns;

    auto readAssign = [&]() -> QUpdate::assign_t {
        if (LexemeDataToStr(*curLexemeItr_) == "," || LexemeDataToStr(*curLexemeItr_) == "where") {
            throw CompileException(*curLexemeItr_, "expected assignment");
        }
        if (curLexemeItr_->type != ELexemeType::Identifier) {
            throw CompileException(*curLexemeItr_, "expected identifier");
        }
        QUpdate::assign_t assign = {curLexemeItr_->str, nullptr};
        ReadLexeme();
        if (LexemeDataToStr(*curLexemeItr_) != "=") {
            throw CompileException(*curLexemeItr_, R"(expected '=')");
        }
        ReadLexeme();
        assign.second = Compiler::Expression();
        return assign;
    };

    while (true) {
        assigns.push_back(readAssign());
        if (LexemeDataToStr(*curLexemeItr_) == ",") {
            ReadLexeme();
        } else if (LexemeDataToStr(*curLexemeItr_) == "where") {
            break;
        }
    }

    return assigns;
}

