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
#include "Parser/Lexeme.h"

enum class EQueryType : int32_t {
    QInsert,
    QCreateTable,
    QCreateIndex,
    QSelect,
    QUpdate,
    QJoin,
    QTable,
};

class RuntimeException : std::exception {
public:
    RuntimeException() = default;

    explicit RuntimeException(std::string ctx) {
        str_ = "[RE] " + std::move(ctx);
    }

    [[nodiscard]] const char* what() const noexcept override {
        return str_.c_str();
    }
private:
    std::string str_;
};

class OperationNode {
public:
    std::shared_ptr<OperationNode> left;
    std::shared_ptr<OperationNode> right;
    Lexeme lexeme;

    explicit OperationNode(Lexeme lexeme) {
        left = nullptr;
        right = nullptr;
        this->lexeme = std::move(lexeme);
    }

    Table::cell_t getResult();
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

class QSelect : public IQuery {
public:
    void exec() override;
    EQueryType getType() final { return type_; }

private:
    EQueryType type_;
    std::vector<std::string> colNames_;
    std::shared_ptr<OperationNode> conditionExpr_;
};

#endif //DATABASE_QUERIES_H
