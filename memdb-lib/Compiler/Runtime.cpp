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
    auto tmp = tablesQueue_.top();
    tablesQueue_.pop();
    return tmp;
}

void Runtime::putQuery(const std::shared_ptr<IQuery>& query_ptr) {
    queriesQueue_.push(query_ptr);
}

void Runtime::putTable(const std::shared_ptr<Table>& table_ptr) {
    tablesQueue_.push(table_ptr);
}

void Runtime::Run(std::map<std::string, std::shared_ptr<Table>>& tRegistry) {
    while (!queriesQueue_.empty()) {
        auto query = getQuery();
        if (query->getType() == EQueryType::QTable) {
            std::dynamic_pointer_cast<QTable>(query)->exec(*this, tRegistry);
        } else if (query->getType() == EQueryType::QCreateTable) {
            std::dynamic_pointer_cast<QCreateTable>(query)->exec(*this, tRegistry);
        } else {
            query->exec(*this);
        }
    }
}
