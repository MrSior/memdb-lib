#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <sstream>

#define DB_DEBUG
#include "memdb-lib.h"

TEST_CASE("Create table test", "[basic][query]") {
    memdb::Database db;

    SECTION("All column types") {
        auto res = db.execute(R"(create table users (id: int32, login: string[32], hash: bytes[8], is_admin: bool))");
        INFO("" << res->GetString());
        REQUIRE(res->isOk() == true);

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        REQUIRE(tRegistry.size() == 1);
        REQUIRE(tRegistry.contains("users") == true);

        auto tHeader = tRegistry["users"]->getHeader();
        REQUIRE(tHeader.tName == "users");
        REQUIRE(tHeader.columns.size() == 4);

        auto columns = tHeader.columns;
        REQUIRE(columns[0].name == "id");
        REQUIRE(columns[0].type == EDbType::INT32);


        REQUIRE(columns[1].name == "login");
        REQUIRE(columns[1].type == EDbType::STRING);
        REQUIRE(columns[1].size == 32);

        REQUIRE(columns[2].name == "hash");
        REQUIRE(columns[2].type == EDbType::BYTES);
        REQUIRE(columns[2].size == 8);

        REQUIRE(columns[3].name == "is_admin");
        REQUIRE(columns[3].type == EDbType::BOOL);
    }

    SECTION("Column attributes") {
        auto res = db.execute(R"(create table users ({autoincrement, key, unique}id: int32))");
        INFO("" << res->GetString());
        REQUIRE(res->isOk() == true);

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        REQUIRE(tRegistry.size() == 1);
        REQUIRE(tRegistry.contains("users") == true);

        auto tHeader = tRegistry["users"]->getHeader();
        auto columns = tHeader.columns;
        REQUIRE(columns[0].name == "id");
        REQUIRE(columns[0].type == EDbType::INT32);
        REQUIRE(columns[0].attributes.size() == 3);

        auto attrs = columns[0].attributes;
        REQUIRE(std::find_if(attrs.begin(), attrs.end(), [](const EAttributes& attr) {
            return attr == EAttributes::AUTOINCREMENT;
        }) != attrs.end());
        REQUIRE(std::find_if(attrs.begin(), attrs.end(), [](const EAttributes& attr) {
            return attr == EAttributes::KEY;
        }) != attrs.end());
        REQUIRE(std::find_if(attrs.begin(), attrs.end(), [](const EAttributes& attr) {
            return attr == EAttributes::UNIQUE;
        }) != attrs.end());
    }

    SECTION("default values") {
        auto res = db.execute(R"(create table users (id: int32 = 1, login: string[32] = "abc", hash: bytes[8] = 0x9ab, is_admin: bool = true))");
        INFO("" << res->GetString());
        REQUIRE(res->isOk());

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        REQUIRE(tRegistry.size() == 1);
        REQUIRE(tRegistry.contains("users") == true);

        auto tHeader = tRegistry["users"]->getHeader();
        REQUIRE(tHeader.tName == "users");
        REQUIRE(tHeader.columns.size() == 4);

        auto columns = tHeader.columns;
        REQUIRE(columns[0].name == "id");
        REQUIRE(columns[0].type == EDbType::INT32);
        auto defaultVal = columns[0].defaultValue;
        REQUIRE(std::holds_alternative<int32_t>(defaultVal));
        REQUIRE(std::get<int32_t>(defaultVal) == 1);


        REQUIRE(columns[1].name == "login");
        REQUIRE(columns[1].type == EDbType::STRING);
        REQUIRE(columns[1].size == 32);
        defaultVal = columns[1].defaultValue;
        REQUIRE(std::holds_alternative<std::string>(defaultVal) == true);
        REQUIRE(std::get<std::string>(defaultVal) == "abc");

        REQUIRE(columns[2].name == "hash");
        REQUIRE(columns[2].type == EDbType::BYTES);
        REQUIRE(columns[2].size == 8);
        defaultVal = columns[2].defaultValue;
        REQUIRE(std::holds_alternative<bytes>(defaultVal));
        auto val = std::get<bytes>(defaultVal);
        REQUIRE(std::string(val.begin(), val.end()) == "0x9ab");

        REQUIRE(columns[3].name == "is_admin");
        REQUIRE(columns[3].type == EDbType::BOOL);
        defaultVal = columns[3].defaultValue;
        REQUIRE(std::holds_alternative<bool>(defaultVal));
        REQUIRE(std::get<bool>(defaultVal) == true);
    }

    SECTION("combined") {
        auto res = db.execute(R"(create table users ({autoincrement, key, unique}id: int32 = 1, is_admin:bool))");
        INFO("" << res->GetString());
        REQUIRE(res->isOk());

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        REQUIRE(tRegistry.size() == 1);
        REQUIRE(tRegistry.contains("users") == true);

        auto tHeader = tRegistry["users"]->getHeader();
        REQUIRE(tHeader.tName == "users");
        REQUIRE(tHeader.columns.size() == 2);

        auto columns = tHeader.columns;
        REQUIRE(columns[0].name == "id");
        REQUIRE(columns[0].type == EDbType::INT32);
        REQUIRE(columns[0].attributes.size() == 3);

        auto attrs = columns[0].attributes;
        REQUIRE(std::find_if(attrs.begin(), attrs.end(), [](const EAttributes& attr) {
            return attr == EAttributes::AUTOINCREMENT;
        }) != attrs.end());
        REQUIRE(std::find_if(attrs.begin(), attrs.end(), [](const EAttributes& attr) {
            return attr == EAttributes::KEY;
        }) != attrs.end());
        REQUIRE(std::find_if(attrs.begin(), attrs.end(), [](const EAttributes& attr) {
            return attr == EAttributes::UNIQUE;
        }) != attrs.end());

        REQUIRE(columns[0].name == "id");
        REQUIRE(columns[0].type == EDbType::INT32);
        auto defaultVal = columns[0].defaultValue;
        REQUIRE(std::holds_alternative<int32_t>(defaultVal));
        REQUIRE(std::get<int32_t>(defaultVal) == 1);
    }
}


TEST_CASE("Insert table test", "[basic][query]") {
    memdb::Database db;
    auto res = db.execute(R"(create table users (id: int32 = 1, login: string[32] = "abc", hash: bytes[8] = 0x9ab, is_admin: bool = true))");

    SECTION("Insert <name> = <val>") {
        res = db.execute(R"(insert (id = 2, hash = 0xabc, login = "cde", is_admin = false) to users)");
        INFO("" << res->GetString());
        REQUIRE(res->isOk() == true);

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        auto table = tRegistry["users"];
        auto row = table->getRow(0);

        REQUIRE(std::holds_alternative<int32_t>(row[0]) == true);
        REQUIRE(std::get<int32_t>(row[0]) == 2);

        REQUIRE(std::holds_alternative<std::string>(row[1]) == true);
        REQUIRE(std::get<std::string>(row[1]) == "cde");

        REQUIRE(std::holds_alternative<std::vector<char>>(row[2]) == true);
        auto val = std::get<std::vector<char>>(row[2]);
        REQUIRE(std::string(val.begin(), val.end()) == "0xabc");

        REQUIRE(std::holds_alternative<bool>(row[3]) == true);
        REQUIRE(std::get<bool>(row[3]) == false);
    }

    SECTION("Insert (<val>, <val>)") {
        res = db.execute(R"(insert (2, "cde", 0xabc, false) to users)");
        INFO("" << res->GetString());
        REQUIRE(res->isOk() == true);

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        auto table = tRegistry["users"];
        auto row = table->getRow(0);

        REQUIRE(std::holds_alternative<int32_t>(row[0]) == true);
        REQUIRE(std::get<int32_t>(row[0]) == 2);

        REQUIRE(std::holds_alternative<std::string>(row[1]) == true);
        REQUIRE(std::get<std::string>(row[1]) == "cde");

        REQUIRE(std::holds_alternative<std::vector<char>>(row[2]) == true);
        auto val = std::get<std::vector<char>>(row[2]);
        REQUIRE(std::string(val.begin(), val.end()) == "0xabc");

        REQUIRE(std::holds_alternative<bool>(row[3]) == true);
        REQUIRE(std::get<bool>(row[3]) == false);
    }

    SECTION("Insert default values") {
        res = db.execute(R"(insert (,,,) to users)");
        INFO("" << res->GetString());
        REQUIRE(res->isOk() == true);

        auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
        auto table = tRegistry["users"];
        auto row = table->getRow(0);

        REQUIRE(std::holds_alternative<int32_t>(row[0]) == true);
        REQUIRE(std::get<int32_t>(row[0]) == 1);

        REQUIRE(std::holds_alternative<std::string>(row[1]) == true);
        REQUIRE(std::get<std::string>(row[1]) == "abc");

        REQUIRE(std::holds_alternative<std::vector<char>>(row[2]) == true);
        auto val = std::get<std::vector<char>>(row[2]);
        REQUIRE(std::string(val.begin(), val.end()) == "0x9ab");

        REQUIRE(std::holds_alternative<bool>(row[3]) == true);
        REQUIRE(std::get<bool>(row[3]) == true);
    }

    SECTION("Do not allow mix of two styles") {
        res = db.execute(R"(insert (id = 3,,,) to users)");
        REQUIRE(res->isOk() == false);
    }

    SECTION("Check invalid type") {
        res = db.execute(R"(insert ("3",,,) to users)");
        REQUIRE(res->isOk() == false);
    }
}

TEST_CASE("Select from table test", "[basic][query]") {
    memdb::Database db;
    auto res = db.execute(R"(create table users (id: int32 = 1, login: string[32] = "abc", hash: bytes[8] = 0x9ab, is_admin: bool = true))");
    db.execute(R"(insert (id = 2, hash = 0xabc, login = "abc_333", is_admin = false) to users)");
    db.execute(R"(insert (id = 0, hash = 0xabc, login = "cde", is_admin = true) to users)");
    db.execute(R"(insert (id = 1, hash = 0xabc, login = "abc", is_admin = true) to users)");

    res = db.execute(R"(select id, hash, login, is_admin from users where id % 2 = 0 && |login| < 4)");
    INFO("" << res->GetString());
    REQUIRE(res->isOk() == true);
    auto table = res->GetTable().value();
    REQUIRE(table.getSize() == 1);
    REQUIRE(table.getCell<int32_t>(table.getRow(0), "id") == 0);
}

TEST_CASE("Update table test", "[basic][query]") {
    memdb::Database db;
    auto res = db.execute(R"(create table users (id: int32 = 1, login: string[32] = "abc", hash: bytes[8] = 0x9ab, is_admin: bool = true))");
    db.execute(R"(insert (id = 2, hash = 0xabc, login = "abc_333", is_admin = false) to users)");
    db.execute(R"(insert (id = 0, hash = 0xabc, login = "cde", is_admin = true) to users)");
    db.execute(R"(insert (id = 1, hash = 0xabc, login = "abc", is_admin = true) to users)");

    res = db.execute(R"(update users set id = 2 * id + 3, login = login + "_changed" where id > 0)");
    INFO(res->GetString());
    REQUIRE(res->isOk() == true);

    auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
    auto table = tRegistry["users"];

    REQUIRE(table->getSize() == 3);
    REQUIRE(table->getCell<int>(table->getRow(0), "id") == 7);
    REQUIRE(table->getCell<std::string>(table->getRow(0), "login") == "abc_333_changed");

    REQUIRE(table->getCell<int>(table->getRow(1), "id") == 0);
    REQUIRE(table->getCell<std::string>(table->getRow(1), "login") == "cde");

    REQUIRE(table->getCell<int>(table->getRow(2), "id") == 5);
    REQUIRE(table->getCell<std::string>(table->getRow(2), "login") == "abc_changed");
}

TEST_CASE("Delete from table test", "[basic][query]") {
    memdb::Database db;
    auto res = db.execute(R"(create table users (id: int32 = 1, login: string[32] = "abc", hash: bytes[8] = 0x9ab, is_admin: bool = true))");
    db.execute(R"(insert (id = 2, hash = 0xabc, login = "abc_333", is_admin = false) to users)");
    db.execute(R"(insert (id = 0, hash = 0xabc, login = "cde", is_admin = true) to users)");
    db.execute(R"(insert (id = 1, hash = 0xabc, login = "abc", is_admin = true) to users)");

    res = db.execute(R"(delete users where id > 0 && |login| > 3)");
    INFO(res->GetString());
    REQUIRE(res->isOk() == true);

    auto tRegistry = memdb::DatabaseCtl::getTableRegistry(db);
    auto table = tRegistry["users"];

    REQUIRE(table->getSize() == 2);
    REQUIRE(table->getCell<int>(table->getRow(0), "id") == 0);
    REQUIRE(table->getCell<std::string>(table->getRow(0), "login") == "cde");

    REQUIRE(table->getCell<int>(table->getRow(1), "id") == 1);
    REQUIRE(table->getCell<std::string>(table->getRow(1), "login") == "abc");
}

TEST_CASE("Join test", "[basic][lang-construction]") {
    memdb::Database db;

    SECTION("Join without condition") {
        auto res = db.execute(R"(create table users (id: int32, login: string[32], isAdmin:bool))");
        db.execute(R"(create table posts (id: int32, text: string[32], userId:int32))");
        db.execute(R"(insert (1, "Jon", true) to users)");
        db.execute(R"(insert (2, "Peter", false) to users)");
        db.execute(R"(insert (3, "Ban", true) to users)");

        db.execute(R"(insert (1, "Hello", 2) to posts)");
        db.execute(R"(insert (2, "Hi", 1) to posts)");
        db.execute(R"(insert (3, "Privet", 3) to posts)");

        std::stringstream ss;
        res = db.execute(
                "select users.id, users.login, users.isAdmin, posts.id, posts.text, posts.userId from users join posts on true where true");
        INFO(res->GetString());
        REQUIRE(res->isOk() == true);
        REQUIRE(res->GetTable() != std::nullopt);

        memdb::printTable(ss, res->GetTable().value());
        REQUIRE(ss.str() ==
                "+--------------------+--------------------+--------------------+--------------------+--------------------+--------------------+\n"
                "|users.id            |users.login         |users.isAdmin       |posts.id            |posts.text          |posts.userId        |\n"
                "|(int32)             |(string)            |(bool)              |(int32)             |(string)            |(int32)             |\n"
                "+--------------------+--------------------+--------------------+--------------------+--------------------+--------------------+\n"
                "|1                   |Jon                 |true                |1                   |Hello               |2                   |\n"
                "|1                   |Jon                 |true                |2                   |Hi                  |1                   |\n"
                "|1                   |Jon                 |true                |3                   |Privet              |3                   |\n"
                "|2                   |Peter               |false               |1                   |Hello               |2                   |\n"
                "|2                   |Peter               |false               |2                   |Hi                  |1                   |\n"
                "|2                   |Peter               |false               |3                   |Privet              |3                   |\n"
                "|3                   |Ban                 |true                |1                   |Hello               |2                   |\n"
                "|3                   |Ban                 |true                |2                   |Hi                  |1                   |\n"
                "|3                   |Ban                 |true                |3                   |Privet              |3                   |\n"
                "+--------------------+--------------------+--------------------+--------------------+--------------------+--------------------+\n");
    }

    SECTION("Join with condition 1") {
        auto res = db.execute(R"(create table users (id: int32, login: string[32], isAdmin:bool))");
        db.execute(R"(create table posts (id: int32, text: string[32], userId:int32))");
        db.execute(R"(insert (0, "Jon", true) to users)");
        db.execute(R"(insert (1, "Peter", false) to users)");
        db.execute(R"(insert (2, "Ban", true) to users)");

        db.execute(R"(insert (1, "Hello", 2) to posts)");
        db.execute(R"(insert (2, "Hi", 1) to posts)");
        db.execute(R"(insert (3, "Privet", 3) to posts)");

        std::stringstream ss;
        res = db.execute("select users.id, users.login, posts.id, posts.text from users join posts on users.id < posts.id where true");
        INFO(res->GetString());
        REQUIRE(res->isOk() == true);
        REQUIRE(res->GetTable() != std::nullopt);

        memdb::printTable(ss, res->GetTable().value());
        REQUIRE(ss.str() == "+--------------------+--------------------+--------------------+--------------------+\n"
                            "|users.id            |users.login         |posts.id            |posts.text          |\n"
                            "|(int32)             |(string)            |(int32)             |(string)            |\n"
                            "+--------------------+--------------------+--------------------+--------------------+\n"
                            "|0                   |Jon                 |1                   |Hello               |\n"
                            "|0                   |Jon                 |2                   |Hi                  |\n"
                            "|0                   |Jon                 |3                   |Privet              |\n"
                            "|1                   |Peter               |2                   |Hi                  |\n"
                            "|1                   |Peter               |3                   |Privet              |\n"
                            "|2                   |Ban                 |3                   |Privet              |\n"
                            "+--------------------+--------------------+--------------------+--------------------+\n");
    }
}

TEST_CASE("Nested queries test", "[basic][queries]") {
    memdb::Database db;

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
    INFO(res->GetString());
    REQUIRE(res->isOk() == true);
    REQUIRE(res->GetTable() != std::nullopt);

    std::stringstream ss;
    memdb::printTable(ss, res->GetTable().value());
    REQUIRE(ss.str() == "+---------------------------+---------------------------+---------------------------+---------------------------+\n"
                        "|posts.text                 |posts.userId               |usersqueries.queries.text  |usersqueries.users.login   |\n"
                        "|(string)                   |(int32)                    |(string)                   |(string)                   |\n"
                        "+---------------------------+---------------------------+---------------------------+---------------------------+\n"
                        "|Hello                      |2                          |Add book                   |Peter                      |\n"
                        "|Hi                         |1                          |Buy apple                  |Jon                        |\n"
                        "|Privet                     |3                          |Open window                |Ban                        |\n"
                        "+---------------------------+---------------------------+---------------------------+---------------------------+\n");
}


TEST_CASE("Error check Create table", "[basic][query]") {
    memdb::Database db;

    SECTION("misspelling") {
        auto res = db.execute(R"(create table users ({autoincrementsdfsdf, key, unique}id: int32 = 1, is_admin:bool))");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
        REQUIRE(res->GetString() == "Fail: [CE] l:1 p:40 lex: autoincrementsdfsdf ctx: expected column attribute");
    }

    SECTION("missed separator") {
        auto res = db.execute(R"(create table users ({autoincrement, key, unique}id: int32 = 1 is_admin:bool))");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
        REQUIRE(res->GetString() == "Fail: [CE] l:1 p:70 lex: is_admin ctx: expected ',' as separator or ')'");
    }

    SECTION("invalid default value") {
        auto res = db.execute(R"(create table users ({autoincrement, key, unique}id: int32 = "abc", is_admin:bool))");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
        REQUIRE(res->GetString() == "Fail: [CE] l:1 p:65 lex: abc ctx: incorrect default value");
    }
}

TEST_CASE("Error check Insert table", "[basic][query]") {
    memdb::Database db;
    auto res = db.execute(R"(create table users (id: int32, login: string[5] = "abc", hash: bytes[4] = 0x9ab, is_admin: bool = true))");

    SECTION("invalid value type") {
        res = db.execute(R"(insert (id = "123", hash = 0xabc, login = "50", is_admin = false) to users)");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
    }

    SECTION("invalid col name") {
        res = db.execute(R"(insert (myNumber = 123, hash = 0xabc, login = "50", is_admin = false) to users)");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
    }

    SECTION("String col overflow") {
        res = db.execute(R"(insert (id = 123, hash = 0xabc, login = "1234567", is_admin = false) to users)");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
    }

    SECTION("Bytes col overflow") {
        res = db.execute(R"(insert (id = 123, hash = 0x123456789a, login = "a", is_admin = false) to users)");
        INFO(res->GetString());
        REQUIRE(res->isOk() == false);
    }
}