cmake_minimum_required(VERSION 3.27)
project(Database)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/memdb-lib)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

add_library(lib STATIC memdb-lib/memdb-lib.h
        memdb-lib/memdb-lib.cpp
        memdb-lib/Parser/Stream.cpp
        memdb-lib/Parser/Stream.h
        memdb-lib/Parser/LexemeParser.cpp
        memdb-lib/Parser/LexemeParser.h
        memdb-lib/Parser/Lexeme.cpp
        memdb-lib/Parser/Lexeme.h
        memdb-lib/Compiler/Compiler.cpp
        memdb-lib/Compiler/Compiler.h
        memdb-lib/Compiler/Runtime.cpp
        memdb-lib/Compiler/Runtime.h
        memdb-lib/Compiler/Queries.cpp
        memdb-lib/Compiler/Queries.h
        memdb-lib/Compiler/Table.cpp
        memdb-lib/Compiler/Table.h)
add_executable(db-app app/app.cpp)
add_executable(lib-tests lib-tests/tests.cpp)

target_link_libraries(db-app PRIVATE lib)
target_link_libraries(lib-tests PRIVATE Catch2::Catch2WithMain)

add_custom_target(run-lib-tests
    COMMAND lib-tests "[factorial]"
    DEPENDS lib-tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "running tests with tag [factorial]"
)