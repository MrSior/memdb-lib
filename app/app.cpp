//
// Created by Семён on 12.11.2024.
//

#include <iostream>
#include <iomanip>
#include "memdb-lib.h"

int main() {
    memdb::Database db;

//    auto res = db.execute("create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)");
    auto res = db.execute("create table users ({autoincrement} id : int32, is_admin: bool = false)");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("insert (0,true) to users");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("insert (,) to users");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("insert (is_admin = false, id = 30) to users");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("select id, is_admin from users where true");
    if (res->isOk()) {
        if (res->GetTable() != std::nullopt) {
            memdb::printTable(std::cout, res->GetTable().value());
        }
    } else {
        std::cout << res->GetString() << std::endl;
    }

    return 0;
}