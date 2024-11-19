//
// Created by Семён on 17.11.2024.
//

#include "Runtime.h"

std::shared_ptr<IQuery> Runtime::getQuery() {
    auto tmp = queriesQueue_.front();
    queriesQueue_.pop();
    return tmp;
}

std::shared_ptr<Table> Runtime::getTable() {
    auto tmp = tablesStack_.top();
    tablesStack_.pop();
    return tmp;
}

void Runtime::putQuery(const std::shared_ptr<IQuery>& query_ptr) {
    queriesQueue_.push(query_ptr);
}

void Runtime::putTable(const std::shared_ptr<Table>& table_ptr) {
    tablesStack_.push(table_ptr);
}
