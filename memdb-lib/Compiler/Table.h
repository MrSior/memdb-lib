//
// Created by Семён on 17.11.2024.
//

#ifndef DATABASE_TABLE_H
#define DATABASE_TABLE_H

#include <cinttypes>
#include <string>
#include <vector>

typedef std::vector<char> bytes;

enum class EDbType : int32_t {
    INT32,
    BOOl,
    STRING,
    BYTES,
};

enum class EAttributes : int32_t {
    UNIQUE,
    AUTOINCREMENT,
    KEY,
};

class Table {

};

class Column {
public:
    std::string name;
    EDbType type;
    size_t size;
    std::vector<EAttributes> attributes;
    std::variant<int, bool, std::string, bytes> defaultValue;
};


#endif //DATABASE_TABLE_H
