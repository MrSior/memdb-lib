//
// Created by Семён on 17.11.2024.
//

#include "Queries.h"
#include "Runtime.h"
#include <type_traits>
#include <functional>
#include <unordered_map>

void QTable::exec(Runtime& rt) {
    throw RuntimeException("QTable exec() must be called with a table registry reference parameter");
}

void QTable::exec(Runtime& rt, std::map<std::string, std::shared_ptr<Table>> &tRegistry) {
    if (!tRegistry.contains(tableName_)) {
        throw RuntimeException("table: " + tableName_ + " doesn't exist");
    }
    rt.putTable(tRegistry[tableName_]);
}

void QCreateTable::exec(Runtime& rt) {
    throw RuntimeException("QCreateTable exec() must be called with a table registry reference parameter");
}

void QCreateTable::exec(Runtime &rt, std::map<std::string, std::shared_ptr<Table>> &tRegistry) {
    tRegistry.insert({header_.tName, std::make_shared<Table>(header_)});
}

void QInsert::exec(Runtime& rt) {
    auto table = rt.getTable();
    auto tHeader = table->getHeader();
    if (tHeader.columns.size() != values_.size()) {
        throw RuntimeException("invalid number of values in insert");
    }
    Table::row_t row(tHeader.columns.size());
    bool isWithNames = true;
    if (values_.front().first.empty()) {
        isWithNames = false;
    }
    for (int idx = 0; idx < values_.size(); ++idx) {
        if (isWithNames) {
            if (values_[idx].first.empty()) {
                throw RuntimeException("All values must be in format <name> = <value>");
            }

            for (int i = 0; i < tHeader.columns.size(); ++i) {
                if (tHeader.columns[i].name == values_[idx].first) {
                    if (std::holds_alternative<int32_t>(values_[idx].second)) {
                        row[i] = std::get<int32_t>(values_[idx].second);
                        continue;
                    }
                    if (std::holds_alternative<bool>(values_[idx].second)) {
                        row[i] = std::get<bool>(values_[idx].second);
                        continue;
                    }
                    if (std::holds_alternative<std::string>(values_[idx].second)) {
                        row[i] = std::get<std::string>(values_[idx].second);
                        continue;
                    }
                    if (std::holds_alternative<bytes>(values_[idx].second)) {
                        row[i] = std::get<bytes>(values_[idx].second);
                        continue;
                    }
                }
            }
        } else {
            if (std::holds_alternative<std::nullptr_t>(values_[idx].second)) {
                bool flag = false;
                for (const auto& attr : tHeader.columns[idx].attributes) {
                    if (attr == EAttributes::AUTOINCREMENT) {
                        flag = true;
                        if (table->getSize() == 0) {
                            throw RuntimeException("column: " + tHeader.columns[idx].name +
                                                    " has AUTOINCREMENT attr but the table is empty");
                        }
                        if (std::holds_alternative<int32_t>(table->back()[idx])) {
                            row[idx] = std::get<int32_t>(table->back()[idx]) + 1;
                        } else {
                            throw RuntimeException("AUTOINCREMENT attr for not-int column");
                        }
                        break;
                    }
                }

                if (!flag) {
                    if (tHeader.columns[idx].isHasDefault) {
                        row[idx] = tHeader.columns[idx].defaultValue;
                    } else {
                        throw RuntimeException("column: " + tHeader.columns[idx].name + " doesn't have default value");
                    }
                }
            } else if (values_[idx].first.empty()) {
                if (values_[idx].second.index() == (int32_t)tHeader.columns[idx].type + 1) {
                    if (std::holds_alternative<int32_t>(values_[idx].second)) {
                        row[idx] = std::get<int32_t>(values_[idx].second);
                        continue;
                    }
                    if (std::holds_alternative<bool>(values_[idx].second)) {
                        row[idx] = std::get<bool>(values_[idx].second);
                        continue;
                    }
                    if (std::holds_alternative<std::string>(values_[idx].second)) {
                        row[idx] = std::get<std::string>(values_[idx].second);
                        continue;
                    }
                    if (std::holds_alternative<bytes>(values_[idx].second)) {
                        row[idx] = std::get<bytes>(values_[idx].second);
                        continue;
                    }
                } else {
                    throw RuntimeException("column: " + tHeader.columns[idx].name + " and value has different types");
                }
            } else {
                throw RuntimeException("All values must in format (<value>,<value>,<value> ...)");
            }
        }
    }
    table->Insert(row);
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

            auto processOperation = [](const std::string& op, const Table::cell_t& a, const Table::cell_t& b) -> int32_t {
                static const std::unordered_map<std::string, std::function<int32_t(int32_t, int32_t)>> operations = {
                        {"+", [](int32_t a, int32_t b) { return a + b; }},
                        {"-", [](int32_t a, int32_t b) { return a - b; }},
                        {"*", [](int32_t a, int32_t b) { return a * b; }},
                        {"/", [](int32_t a, int32_t b) { if (b == 0) throw RuntimeException("Division by zero"); return a / b; }},
                        {"%", [](int32_t a, int32_t b) { if (b == 0) throw RuntimeException("Modulo by zero"); return a % b; }},
                };

                auto it = operations.find(op);
                if (it != operations.end()) {
                    if (!std::holds_alternative<int32_t>(a)) {
                        RuntimeException("Unsupported type for math operation: " + op);
                    }
                    return it->second(std::get<int32_t>(a), std::get<int32_t>(b));
                }
                throw RuntimeException("Unsupported operation: " + op);
            };

            auto processComparison = [](const std::string& op, const Table::cell_t& a, const Table::cell_t& b) -> bool {
                static std::function<bool(const std::string&, const Table::cell_t&, const Table::cell_t&)> self = nullptr;

                self = [](const std::string& op, const Table::cell_t& a, const Table::cell_t& b) -> bool {
                    using comparison_func = std::function<bool(const Table::cell_t&, const Table::cell_t&)>;

                    static const std::unordered_map<std::string, comparison_func> comparisons = {
                            {"=", [](const Table::cell_t& a, const Table::cell_t& b) {
                                if (std::holds_alternative<int32_t>(a)) return std::get<int32_t>(a) == std::get<int32_t>(b);
                                if (std::holds_alternative<bool>(a)) return std::get<bool>(a) == std::get<bool>(b);
                                if (std::holds_alternative<std::string>(a)) return std::get<std::string>(a) == std::get<std::string>(b);
                                if (std::holds_alternative<bytes>(a)) {
                                    return std::string(std::get<bytes>(a).begin(), std::get<bytes>(a).end()) ==
                                           std::string(std::get<bytes>(b).begin(), std::get<bytes>(b).end());
                                }
                                throw RuntimeException("Unsupported type for == comparison");
                            }},
                            {"!=", [](const Table::cell_t& a, const Table::cell_t& b) {
                                return !self("=", a, b);
                            }},
                            {"<", [](const Table::cell_t& a, const Table::cell_t& b) {
                                if (std::holds_alternative<int32_t>(a)) return std::get<int32_t>(a) < std::get<int32_t>(b);
                                if (std::holds_alternative<bool>(a)) return std::get<bool>(a) < std::get<bool>(b);
                                if (std::holds_alternative<std::string>(a)) return std::get<std::string>(a) < std::get<std::string>(b);
                                if (std::holds_alternative<bytes>(a)) {
                                    return std::string(std::get<bytes>(a).begin(), std::get<bytes>(a).end()) <
                                           std::string(std::get<bytes>(b).begin(), std::get<bytes>(b).end());
                                }
                                throw RuntimeException("Unsupported type for < comparison");
                            }},
                            {"<=", [](const Table::cell_t& a, const Table::cell_t& b) {
                                return self("<", a, b) || self("=", a, b);
                            }},
                            {">", [](const Table::cell_t& a, const Table::cell_t& b) {
                                return !self("<=", a, b);
                            }},
                            {">=", [](const Table::cell_t& a, const Table::cell_t& b) {
                                return !self("<", a, b);
                            }},
                    };

                    auto it = comparisons.find(op);
                    if (it != comparisons.end()) {
                        return it->second(a, b);
                    }
                    throw RuntimeException("Unsupported comparison operation: " + op);
                };

                return self(op, a, b);
            };

            auto processBoolean = [](const std::string& op, const Table::cell_t& a, const Table::cell_t& b) -> bool {
                static const std::unordered_map<std::string, std::function<bool(bool , bool)>> booleans = {
                        {"&&", [](bool a, bool b) { return a && b; }},
                        {"||", [](bool a, bool b) { return a || b; }},
                        {"^^", [](bool a, bool b) { return a ^ b; }},
                };

                auto it = booleans.find(op);
                if (it != booleans.end()) {
                    if (!std::holds_alternative<bool>(a)) {
                        RuntimeException("Unsupported type for math operation: " + op);
                    }
                    return it->second(std::get<bool>(a), std::get<bool>(b));
                }
                throw RuntimeException("Unsupported operation: " + op);
            };

            auto op = LexemeDataToStr(lexeme);
            if (op == "<" || op == "<=" || op == ">" || op == ">=" || op == "!=" || op == "=") {
                return processComparison(op, leftRes, rightRes);
            }
            if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
                return processOperation(op, leftRes, rightRes);
            }
            if (op == "&&" || op == "||" || op == "^^") {
                return processBoolean(op, leftRes, rightRes);
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
    throw RuntimeException("Condition evaluation failed");
}

void QSelect::exec(Runtime& rt) {
    auto table = rt.getTable();
    auto header = table->getHeader();

    THeader resTableHeader;
    std::vector<int> validColsIdx;
    for (const auto& colName : colNames_) {
        for (int idx = 0; idx < header.columns.size(); ++idx) {
            if (colName == header.columns[idx].name) {
                resTableHeader.columns.push_back(header.columns[idx]);
                validColsIdx.push_back(idx);
            }
        }
    }

    auto resTable = std::make_shared<Table>(resTableHeader);
    for (int idx = 0; idx < table->getSize(); ++idx) {
        auto curRow = table->getRow(idx);

        auto conditionRes = conditionExpr_->getResult(header, curRow);
        if (!std::holds_alternative<bool>(conditionRes)) {
            throw RuntimeException("condition expr must be boolean");
        }
        if (std::get<bool>(conditionRes)) {
            Table::row_t row;
            for (auto colInd : validColsIdx) {
                row.push_back(curRow[colInd]);
            }
            resTable->pushRow(row);
        }
    }
    rt.putTable(resTable);
}

void QUpdate::exec(Runtime &rt) {
    auto table = rt.getTable();
    auto header = table->getHeader();

    for (int rowIdx = 0; rowIdx < table->getSize(); ++rowIdx) {
        auto curRow = table->getRow(rowIdx);

        auto conditionRes = conditionExpr_->getResult(header, curRow);
        if (!std::holds_alternative<bool>(conditionRes)) {
            throw RuntimeException("condition expr must be boolean");
        }

        if (std::get<bool>(conditionRes)) {
            for (const auto& assign : assigns_) {
                int colIdx = -1;
                for (int i = 0; i < curRow.size(); ++i) {
                    if (header.columns[i].name == assign.first) {
                        colIdx = i;
                        break;
                    }
                }
                if (colIdx == -1) {
                    throw RuntimeException("unknown column: " + assign.first + " in set block");
                }

                auto assignRes = assign.second->getResult(header, curRow);
                if (curRow[colIdx].index() != assignRes.index()) {
                    throw RuntimeException("column: " + assign.first + " and set expression have different types");
                }
                curRow[colIdx] = assignRes;
            }
            table->setRow(curRow, rowIdx);
        }
    }
}
