//
// Created by Семён on 17.11.2024.
//

#include "Table.h"

std::map<EAttributes, std::string> g_AttrTypeToStr = {
        {EAttributes::AUTOINCREMENT,    "autoincrement"},
        {EAttributes::UNIQUE,           "unique"},
        {EAttributes::KEY,              "key"},
};

std::map<std::string, EAttributes> g_StrToAttrType = {
        {"autoincrement",   EAttributes::AUTOINCREMENT},
        {"unique",          EAttributes::UNIQUE},
        {"key",             EAttributes::KEY},
};

std::map<EDbType, std::string> g_DbTypeToStr = {
        {EDbType::INT32,  "int32"},
        {EDbType::BOOL,   "bool"},
        {EDbType::STRING, "string"},
        {EDbType::BYTES,  "bytes"},
};

std::map<std::string, EDbType> g_StrToDbType = {
        {"int32",   EDbType::INT32},
        {"bool",    EDbType::BOOL},
        {"string",  EDbType::STRING},
        {"bytes",   EDbType::BYTES},
};

void Table::Insert(const Table::row_t& row) {
    table_.push_back(row);
}
