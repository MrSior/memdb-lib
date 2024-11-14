#include "Lexeme.h"
#include <map>
#include <string>

std::string LexemeDataToStr(const Lexeme &lexeme) {
    if ((int32_t)lexeme.type <= (int32_t)ELexemeType::Punctuation &&
        (int32_t)lexeme.type >= (int32_t)ELexemeType::RoundBrack) {
        return std::string() + (char)lexeme.i64;
    }
    if ((int32_t)lexeme.type == (int32_t)ELexemeType::LiteralNum64) {
        return std::to_string(lexeme.i64);
    }
    return lexeme.str;
}

std::map<ELexemeType, std::string> g_LexemeTypeToStr = {
        {ELexemeType::Identifier, "Identifier"},
        {ELexemeType::Keyword, "Keyword"},
        {ELexemeType::Operator, "Operator"},

        {ELexemeType::RoundBrack, "RoundBrack"},
        {ELexemeType::BoxBrack, "BoxBrack"},
        {ELexemeType::CurlyBrack, "CurlyBrack"},
        {ELexemeType::Punctuation, "Punctuation"},

        {ELexemeType::LiteralNum64, "LiteralNum64"},
        {ELexemeType::LiteralStr, "LiteralStr"},
        {ELexemeType::LiteralBytes, "LiteralBytes"},

        {ELexemeType::Null, "Null"},
        {ELexemeType::Invalid, "Invalid"},
        {ELexemeType::ProgramEnd, "ProgramEnd"}};
