//
// Created by Семён on 12.11.2024.
//

#include <iostream>
#include "memdb-lib.h"


int main() {
    memdb::Database db;

//    auto res = db.execute("create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)");
    auto res = db.execute("insert (login = \"vasya\", password_hash = 0xdeadbeefdeadbeef) to users");
    if (res->isOk()) {
        std::cout << "Complete" << std::endl;
    } else {
        std::cout << res->GetString() << std::endl;
    }
    return 0;
}