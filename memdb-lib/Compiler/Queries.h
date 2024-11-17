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
    explicit QCreateTable(std::vector<Column> header) : type_(EQueryType::QCreateTable), header_(std::move(header)) {}

    void exec() override;
    EQueryType getType() final { return type_; }
private:
    EQueryType type_ ;
    std::vector<Column> header_;
};

class QInsert : public IQuery {
public:
    void exec() override;
    EQueryType getType() final { return type_; }

private:
    EQueryType type_;
    std::string tableName_;
    std::vector<std::pair<std::string, std::variant<int, bool, std::string, bytes>>> values_;
};

#endif //DATABASE_QUERIES_H
