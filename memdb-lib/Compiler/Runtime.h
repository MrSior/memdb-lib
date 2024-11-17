//
// Created by Семён on 17.11.2024.
//

#ifndef DATABASE_RUNTIME_H
#define DATABASE_RUNTIME_H

#include "Queries.h"
#include "Table.h"

#include <stack>
#include <memory>

class Runtime {
public:
    Runtime() = default;

    std::shared_ptr<IQuery> getQuery();
    std::shared_ptr<Table> getTable();

    void putQuery(const std::shared_ptr<IQuery>& query_ptr);
    void putTable(const std::shared_ptr<Table>& table_ptr);

private:
    std::stack<std::shared_ptr<IQuery>> queriesStack_;
    std::stack<std::shared_ptr<Table>> tablesStack_;
};


#endif //DATABASE_RUNTIME_H
