//
// Created by Семён on 12.11.2024.
//

#ifndef DATABASE_MEMDB_LIB_H
#define DATABASE_MEMDB_LIB_H

#include <cinttypes>
#include <vector>
#include <map>
#include "Parser/LexemeParser.h"

namespace memdb {

    enum class SQL_TYPE : int32_t {
        INT32,
        BOOL,
        STRING,
        BYTES,
    };

    class Table {
    public:

    private:
        std::map<std::string, SQL_TYPE> header_register_;
    };


    class QueryException : std::exception {
    public:
        QueryException() = default;

        QueryException(std::string str) {
            str_ = str;
        }

        virtual const char* what() {
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
