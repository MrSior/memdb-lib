#include "Lexeme.h"
#include <map>
#include <string>

std::string LexemeDataToStr(const Lexeme &lexeme) {
    if ((int32_t)lexeme.type <= (int32_t)ELexemeType::Punctuation &&
        (int32_t)lexeme.type >= (int32_t)ELexemeType::RoundBrackOp) {
        return std::string() + (char)lexeme.i64;
    }
    if ((int32_t)lexeme.type == (int32_t)ELexemeType::LiteralNum64) {
        return std::to_string(lexeme.i64);
    }
    if ((int32_t)lexeme.type == (int32_t)ELexemeType::LiteralBool) {
        return ((bool)lexeme.i64 ? "true" : "false");
    }
    if (lexeme.type == ELexemeType::ProgramEnd) {
        return "__program_end__";
    }
    return lexeme.str;
}

std::map<ELexemeType, std::string> g_LexemeTypeToStr = {
        {ELexemeType::Identifier, "Identifier"},
        {ELexemeType::Keyword, "Keyword"},
        {ELexemeType::Operator, "Operator"},

        {ELexemeType::RoundBrackOp, "RoundBrackOp"},
        {ELexemeType::BoxBrackOp, "BoxBrackOp"},
        {ELexemeType::CurlyBrackOp, "CurlyBrackOp"},
        {ELexemeType::RoundBrackCl, "RoundBrackCl"},
        {ELexemeType::BoxBrackCl, "BoxBrackCl"},
        {ELexemeType::CurlyBrackCl, "CurlyBrackCl"},
        {ELexemeType::Punctuation, "Punctuation"},

        {ELexemeType::LiteralNum64, "LiteralNum64"},
        {ELexemeType::LiteralBool, "LiteralBool"},
        {ELexemeType::LiteralStr, "LiteralStr"},
        {ELexemeType::LiteralBytes, "LiteralBytes"},

        {ELexemeType::Null, "Null"},
        {ELexemeType::Invalid, "Invalid"},
        {ELexemeType::ProgramEnd, "ProgramEnd"}};
