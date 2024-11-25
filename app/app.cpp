//
// Created by Семён on 12.11.2024.
//

#include <iostream>
#include <iomanip>
#include "memdb-lib.h"

int main() {
    memdb::Database db;

//    auto res = db.execute("create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)");
//    auto res = db.execute(R"(create table users ({autoincrement} id : int32, is_admin: bool = false, name: string[20] = "bbb"))");
//
//
//    res = db.execute("insert (0,true,) to users");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("insert (,,) to users");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("insert (is_admin = false, id = 30, name = \"aaaaa\") to users");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("select id, is_admin from select id, is_admin from users where id < 30 where true");
//    if (res->isOk()) {
//        if (res->GetTable() != std::nullopt) {
//            memdb::printTable(std::cout, res->GetTable().value());
//        }
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//
//    res = db.execute("select id, is_admin, name from users where name < \"abbb\"");
//    if (res->isOk()) {
//        if (res->GetTable() != std::nullopt) {
//            memdb::printTable(std::cout, res->GetTable().value());
//        }
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("create table usersv2 ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute(R"(insert (3, "guest", 0x08b,) to usersv2)");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("select id, is_admin, login, password_hash from usersv2 where true");
//    if (res->isOk()) {
//        if (res->GetTable() != std::nullopt) {
//            memdb::printTable(std::cout, res->GetTable().value());
//        }
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }

    auto res = db.execute(R"(create table users ({autoincrement} id : int32, is_admin: bool = false, name: string[20] = "bbb", hash: bytes[4] = 0xabcd))");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("insert (,true,,) to users");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("insert (,true, \"12345\", 0xabceabce) to users");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

    res = db.execute("update users set is_admin = false, id = id * 2 + 1, name = name + \"_abc\" where |hash| > 2");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }

//    res = db.execute("delete users where id >= 7");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }

    res = db.execute("select id, is_admin, name, hash from users where true");
    if (res->isOk()) {
        if (res->GetTable() != std::nullopt) {
            auto table = res->GetTable().value();
            std::cout << table.getSize() << std::endl;
            for (auto& row : table) {
                auto val = table.getCell<bytes>(row, "hash");
                std::cout << std::string(val.begin(), val.end()) << std::endl;
            }
            memdb::printTable(std::cout, res->GetTable().value());
        }
    } else {
        std::cout << res->GetString() << std::endl;
    }

    return 0;
}