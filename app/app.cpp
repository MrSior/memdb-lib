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

//    auto res = db.execute(R"(create table users ({autoincrement} id : int32, is_admin: bool = false, name: string[20] = "bbb", hash: bytes[4] = 0xabcd))");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("insert (,true,,) to users");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("insert (,true, \"12345\", 0xabceabce) to users");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//
//    res = db.execute("update users set is_admin = false, id = id * 2 + 1, name = name + \"_abc\" where |hash| > 2");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }

//    res = db.execute("delete users where id >= 7");
//    if (res->isOk()) {
//        std::cout << "Complete" << std::endl;
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }

//    res = db.execute("select id, is_admin, name, hash from users where true");
//    if (res->isOk()) {
//        if (res->GetTable() != std::nullopt) {
//            auto table = res->GetTable().value();
//            std::cout << table.getSize() << std::endl;
//            for (auto& row : table) {
//                auto val = table.getCell<bytes>(row, "hash");
//                std::cout << std::string(val.begin(), val.end()) << std::endl;
//            }
//            memdb::printTable(std::cout, res->GetTable().value());
//        }
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }

//    auto res = db.execute(R"(create table users (id: int32 = 1, login: string[32] = "abc", hash: bytes[8] = 0x9ab, is_admin: bool = true))");
//    db.execute(R"(insert (id = 2, hash = 0xabc, login = "abc_333", is_admin = false) to users)");
//    db.execute(R"(insert (id = 0, hash = 0xabc, login = "cde", is_admin = true) to users)");
//    db.execute(R"(insert (id = 1, hash = 0xabc, login = "abc", is_admin = true) to users)");
//    res = db.execute(R"(update users set id = 2 * id + 3, login = login asd + "_changed" where id > 0)");
//    if (res->isOk()) {
//        std::cout << "OK";
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }

    auto res = db.execute(R"(create table users (id: int32, login: string[32], isAdmin:bool))");
    db.execute(R"(create table posts (id: int32, text: string[32], userId:int32))");
    db.execute(R"(create table queries (id: int32, text: string[32], userId:int32))");

    db.execute(R"(insert (1, "Jon", true) to users)");
    db.execute(R"(insert (2, "Peter", false) to users)");
    db.execute(R"(insert (3, "Ban", true) to users)");

    db.execute(R"(insert (1, "Hello", 2) to posts)");
    db.execute(R"(insert (2, "Hi", 1) to posts)");
    db.execute(R"(insert (3, "Privet", 3) to posts)");

    db.execute(R"(insert (1, "Add book", 2) to queries)");
    db.execute(R"(insert (2, "Buy apple", 1) to queries)");
    db.execute(R"(insert (3, "Open window", 3) to queries)");

    res = db.execute("select posts.text, posts.userId, usersqueries.queries.text, usersqueries.users.login from "
                     "posts join users join queries on users.id = queries.userId on posts.userId = usersqueries.users.id "
                     "where true");
    if (res->isOk()) {
        if (res->GetTable() != std::nullopt) {
            memdb::printTable(std::cout, res->GetTable().value());
        }
    } else {
        std::cout << res->GetString() << std::endl;
    }


//    auto res = db.execute(R"(create table users (id: int32 = "abc", is_admin:bool))");
//    if (res->isOk()) {
//
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
//    res = db.execute(R"(insert (,true) to users)");
//    res = db.execute(R"(insert (,true) to users)");
//    res = db.execute(R"(select id from users where true)");
//    if (res->isOk()) {
//        if (res->GetTable() != std::nullopt) {
//            memdb::printTable(std::cout, res->GetTable().value());
//        }
//    } else {
//        std::cout << res->GetString() << std::endl;
//    }
    return 0;
}