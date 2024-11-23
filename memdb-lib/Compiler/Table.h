//
// Created by Семён on 17.11.2024.
//

#ifndef DATABASE_TABLE_H
#define DATABASE_TABLE_H

#include <cinttypes>
#include <string>
#include <utility>
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

class Column {
public:
    Column() = default;

    std::string name;
    EDbType type = EDbType::INT32;
    size_t size = 1;
    bool isHasDefault = false;
    std::vector<EAttributes> attributes;
    std::variant<int32_t, bool, std::string, bytes> defaultValue;
};

class THeader {
public:
    THeader() = default;
    THeader(std::string name, std::vector<Column> columns) : tName(std::move(name)), columns(std::move(columns)) {}

    std::string tName;
    std::vector<Column> columns;
};


class Table {
public:
    using cell_t = std::variant<int32_t, bool, std::string, bytes>;
    using row_t = std::vector<cell_t>;

    Table() = default;
    explicit Table(THeader tHeader) : table_({}), header_(std::move(tHeader)) {};

    void Insert(const row_t& row);
    THeader getHeader() { return header_; }
    row_t back() { return table_.back(); }
    size_t getSize() { return table_.size(); }
    row_t getRow(int idx) { return table_[idx]; }
    void pushRow(const row_t& row) { table_.push_back(row); };
private:
    std::vector<row_t> table_;
    THeader header_;
};

extern std::map<EAttributes, std::string> g_AttrTypeToStr;
extern std::map<std::string, EAttributes> g_StrToAttrType;

extern std::map<EDbType, std::string> g_DbTypeToStr;
extern std::map<std::string, EDbType> g_StrToDbType;

#endif //DATABASE_TABLE_H
