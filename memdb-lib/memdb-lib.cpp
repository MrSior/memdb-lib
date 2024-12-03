//
// Created by Семён on 13.11.2024.
//
#include "memdb-lib.h"
#include <iostream>
#include <iomanip>

std::shared_ptr<memdb::QueryResult> memdb::Database::execute(const std::string &query) {
//    auto* result = new QueryResult();
    auto result = std::make_shared<QueryResult>();
    std::vector<Lexeme> input;
    try {
        input = lexeme_parser_.GetLexemes(query);
//        for (const auto& el : input) {
//            std::cout << g_LexemeTypeToStr[el.type] << " " << LexemeDataToStr(el) << std::endl;
//        }

        Runtime runtime = compiler_.compile(input);
        runtime.Run(tableRegistry_);
        if (runtime.isHasResTable()) {
            result->SetTable(runtime.getTable());
        }
    } catch (const QueryException& ex) {
        result->SetException(ex);
        return result;
    } catch (const CompileException& ex) {
        result->SetException(ex);
        return result;
    } catch (const RuntimeException& ex) {
        result->SetException(ex);
        return result;
    }

    return result;
}

memdb::QueryResult::QueryResult() {
    was_ok_ = true;
    queryResTable_ = std::nullopt;
}

std::string memdb::QueryResult::GetString() {
    std::string res;
    if (was_ok_) {
        res = "Exec Ok";
    } else {
        res = "Fail: " + std::string(query_exception_.what());
    }
    return res;
}

void memdb::QueryResult::SetException(const memdb::QueryException &ex) {
    was_ok_ = false;
    query_exception_ = ex;
}

void memdb::QueryResult::SetException(const CompileException &ex) {
    was_ok_ = false;
    query_exception_ = QueryException(ex);
}

void memdb::QueryResult::SetException(const RuntimeException &ex) {
    was_ok_ = false;
    query_exception_ = QueryException(ex);
}

std::optional<Table> memdb::QueryResult::GetTable() {
    return queryResTable_;
}

void memdb::QueryResult::SetTable(const std::shared_ptr<Table> &tablePtr) {
    queryResTable_ = *tablePtr;
}

void memdb::printTable(std::ostream& os, const Table& table) {
    int columnWidth = 20;

    THeader header = table.getHeader();
    for (auto& col : header.columns) {
        if (columnWidth < col.name.length() + 2) {
            columnWidth = static_cast<int>(col.name.length()) + 2;
        }
    }
    const auto& columns = header.columns;

    os << "+";
    for (int i = 0; i < columns.size(); ++i) {
        os << std::setw(columnWidth) << std::setfill('-') << std::left << "-" << "+";
    }
    os << std::endl;

    os << "|";
    for (const auto& column : columns) {
        os << std::setw(columnWidth) << std::setfill(' ') << std::left << column.name << "|";
    }
    os << std::endl;

    os << "|";
    for (const auto& column : columns) {
        os << std::setw(columnWidth) << std::setfill(' ') << std::left << "(" + g_DbTypeToStr[column.type] + ")" << "|";
    }
    os << std::endl;

    os << "+";
    for (size_t i = 0; i < columns.size(); ++i) {
        os << std::setw(columnWidth) << std::setfill('-') << std::left << "-" << "+";
    }
    os << std::endl;

    for (size_t i = 0; i < table.getSize(); ++i) {
        Table::row_t row = table.getRow(i);
        os << "|";
        for (const auto& cell : row) {
            std::visit([&](auto&& val) {
                std::string cellContent;
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int32_t>) {
                    cellContent = std::to_string(val);
                } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
                    cellContent = val ? "true" : "false";
                } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                    cellContent = val;
                } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bytes>) {
                    cellContent = std::string(val.begin(), val.end());
                }
                os << std::setw(columnWidth) << std::setfill(' ') << std::left << cellContent << "|";
            }, cell);
        }
        os << std::endl;
    }

    os << "+";
    for (size_t i = 0; i < columns.size(); ++i) {
        os << std::setw(columnWidth) << std::setfill('-') << std::left << "-" << "+";
    }
    os << std::endl;
}

