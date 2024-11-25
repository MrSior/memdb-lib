#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#define DB_DEBUG
#include "memdb-lib.h"

//unsigned int factorial( unsigned int number ) {
//    return number <= 1 ? number : factorial(number-1)*number;
//}
//
//TEST_CASE( "Factorials are computed", "[factorial]" ) {
//    REQUIRE( factorial(1) == 1 );
//    REQUIRE( factorial(2) == 2 );
//    REQUIRE( factorial(3) == 6 );
//    REQUIRE( factorial(10) == 3628800 );
//
//    // This find a bug in the factorial implementation,
//    // try uncommenting it.
//    // REQUIRE( factorial(0) == 1 );
//
//    BENCHMARK("fibonacci 25") {
//                                  return factorial(25);
//                              };
//}
//
//TEST_CASE("Example test with tag", "[mytag]") {
//    BENCHMARK("fibonacci 25") {
//                                  return factorial(25);
//                              };
//}


TEST_CASE("Create table test", "[basic][query]") {
    memdb::Database db;

    SECTION("All column types") {
        auto res = db.execute(R"(create table users (id: int32, login: string[32], hash: bytes[8], is_admin: bool))");
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
}