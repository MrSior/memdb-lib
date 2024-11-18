//
// Created by Семён on 12.11.2024.
//

#ifndef DATABASE_MEMDB_LIB_H
#define DATABASE_MEMDB_LIB_H

#include <cinttypes>
#include <utility>
#include <vector>
#include <map>
#include "Parser/LexemeParser.h"
#include "Compiler/Compiler.h"

namespace memdb {
    class QueryException : std::exception {
    public:
        QueryException() = default;

        explicit QueryException(std::string str) {
            str_ = std::move(str);
        }

        explicit QueryException(const CompileException& ex) : QueryException(ex.what()) {}

        [[nodiscard]] const char* what() const noexcept override{
            return str_.c_str();
        }
    private:
        std::string str_;
    };

    class QueryResult {
    public:
        bool isOk() { return was_ok_; }
        std::string GetString();
        void SetException(const QueryException& ex);
        QueryResult();
    private:
        bool was_ok_;
        QueryException query_exception_;
    };

    class Database {
    public:
        Database() = default;

        QueryResult* execute(const std::string& query);
    private:
        LexemeParser lexeme_parser_;
    };
};

#endif //DATABASE_MEMDB_LIB_H
