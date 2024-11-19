//
// Created by Семён on 17.11.2024.
//

#ifndef DATABASE_QUERIES_H
#define DATABASE_QUERIES_H

#include <cinttypes>
#include <string>
#include <utility>
#include <vector>
#include "Table.h"

enum class EQueryType : int32_t {
    QInsert,
    QCreateTable,
    QCreateIndex,
    QSelect,
    QUpdate,
    QJoin,
    QTable,
};

class THeader {
public:
    THeader() = default;
    THeader(std::string name, std::vector<Column> columns) : tName(std::move(name)), columns(std::move(columns)) {}

    std::string tName;
    std::vector<Column> columns;
};

class IQuery {
public:
    virtual void exec() = 0;
    virtual EQueryType getType() = 0;
};

class QTable : public IQuery {
public:
    explicit QTable(std::string tableName) : type_(EQueryType::QTable), tableName_(std::move(tableName)) {}

    void exec() override;
    EQueryType getType() final { return type_; };
private:
    EQueryType type_ ;
    std::string tableName_;
};

class QCreateTable : public IQuery {
public:
    explicit QCreateTable(THeader header) : type_(EQueryType::QCreateTable), header_(std::move(header)) {}

    void exec() override;
    EQueryType getType() final { return type_; }
private:
    EQueryType type_ ;
    THeader header_;
};

class QInsert : public IQuery {
public:
    using queryParam_t = std::pair<std::string, std::variant<std::nullptr_t, int32_t, bool, std::string, bytes>>;
    using queryData_t = std::vector<queryParam_t>;

    QInsert(std::string tName, queryData_t values) : type_(EQueryType::QInsert), tableName_(std::move(tName)), values_(std::move(values)) {}
    void exec() override;

    EQueryType getType() final { return type_; }
private:
    EQueryType type_;
    std::string tableName_;
    queryData_t values_;
};

#endif //DATABASE_QUERIES_H
