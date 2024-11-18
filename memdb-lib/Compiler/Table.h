//
// Created by Семён on 17.11.2024.
//

#ifndef DATABASE_TABLE_H
#define DATABASE_TABLE_H

#include <cinttypes>
#include <string>
#include <vector>
#include <map>
#include <cinttypes>

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
    size_t size = 1;
    bool isHasDefault = false;
    std::vector<EAttributes> attributes;
    std::variant<int32_t, bool, std::string, bytes> defaultValue;
};

extern std::map<EAttributes, std::string> g_AttrTypeToStr;
extern std::map<std::string, EAttributes> g_StrToAttrType;

extern std::map<EDbType, std::string> g_DbTypeToStr;
extern std::map<std::string, EDbType> g_StrToDbType;

#endif //DATABASE_TABLE_H
