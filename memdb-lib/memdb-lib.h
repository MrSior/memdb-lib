//
// Created by Семён on 12.11.2024.
//

#ifndef DATABASE_MEMDB_LIB_H
#define DATABASE_MEMDB_LIB_H

#include <cinttypes>
#include <utility>
#include <vector>
#include <map>
#include <iomanip>
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
        explicit QueryException(const RuntimeException& ex) : QueryException(ex.what()) {}

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
        std::optional<Table> GetTable();
        void SetException(const QueryException& ex);
        void SetException(const CompileException& ex);
        void SetException(const RuntimeException& ex);
        void SetTable(const std::shared_ptr<Table>& tablePtr);
        QueryResult();
    private:
        bool was_ok_;
        QueryException query_exception_;
        std::optional<Table> queryResTable_;
    };

#ifdef DB_DEBUG
    class DatabaseCtl;
#endif

    class Database {
    public:
        Database() = default;

        std::shared_ptr<QueryResult> execute(const std::string& query);

#ifdef DB_DEBUG
        friend DatabaseCtl;
#endif
    private:
        LexemeParser lexeme_parser_;
        Compiler compiler_;
        std::map<std::string, std::shared_ptr<Table>> tableRegistry_;
    };

#ifdef DB_DEBUG
    class DatabaseCtl {
    public:
        static std::map<std::string, std::shared_ptr<Table>> getTableRegistry(Database& db) {
            return db.tableRegistry_;
        }
    };
#endif

    void printTable(std::ostream& os, const Table& table);
};

#endif //DATABASE_MEMDB_LIB_H
