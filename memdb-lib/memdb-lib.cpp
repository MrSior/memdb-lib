//
// Created by Семён on 13.11.2024.
//
#include "memdb-lib.h"
#include <iostream>

memdb::QueryResult* memdb::Database::execute(const std::string &query) {
    auto* result = new QueryResult();
    std::vector<Lexeme> input;
    try {
        input = lexeme_parser_.GetLexemes(query);
        for (const auto& el : input) {
            std::cout << g_LexemeTypeToStr[el.type] << " " << LexemeDataToStr(el) << std::endl;
        }

        Runtime runtime = compiler_.compile(input);
        runtime.Run(tableRegistry_);
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


