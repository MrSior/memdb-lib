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
    BOOL,
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

    THeader getHeader()         const { return header_; }
    row_t   back()              const { return table_.back(); }
    size_t  getSize()           const { return table_.size(); }
    row_t   getRow(int idx)     const { return table_[idx]; }

    void eraseRow(int idx)          { table_.erase(table_.begin() + idx); }
    void setRow(row_t row, int idx) { table_[idx] = std::move(row); }
    void pushRow(const row_t& row)  { table_.push_back(row); };

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = row_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const row_t*;
        using reference = const row_t&;

        explicit Iterator(std::vector<row_t>::const_iterator it) : current_(it) {}

        reference operator*() const { return *current_; }
        pointer operator->() const { return &(*current_); }

        Iterator& operator++() {
            ++current_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const { return current_ == other.current_; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        std::vector<row_t>::const_iterator current_;
    };

    [[nodiscard]] Iterator begin() const { return Iterator(table_.cbegin()); }
    [[nodiscard]] Iterator end() const { return Iterator(table_.cend()); }

    template<typename T>
    T getCell(const row_t& row, const std::string& name) {
        auto it = std::find_if(header_.columns.begin(), header_.columns.end(),
                               [&name](const Column& column) { return column.name == name; });

        if (it == header_.columns.end()) {
            throw std::invalid_argument("column: " + name + " not found");
        }

        size_t colIdx = std::distance(header_.columns.begin(), it);

        if (std::holds_alternative<T>(row[colIdx])) {
            return std::get<T>(row[colIdx]);
        }
        throw std::bad_variant_access();
    }
private:
    std::vector<row_t> table_;
    THeader header_;
};

extern std::map<EAttributes, std::string> g_AttrTypeToStr;
extern std::map<std::string, EAttributes> g_StrToAttrType;

extern std::map<EDbType, std::string> g_DbTypeToStr;
extern std::map<std::string, EDbType> g_StrToDbType;

#endif //DATABASE_TABLE_H
