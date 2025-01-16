//
// Created by Семён on 14.11.2024.
//

#ifndef DATABASE_LEXEMEPARSER_H
#define DATABASE_LEXEMEPARSER_H


#include "Lexeme.h"
#include "Stream.h"
#include <filesystem>
#include <vector>

class LexemeParser {
public:
    LexemeParser() = default;
    std::vector<Lexeme> GetLexemes(const std::string &input);
};


#endif //DATABASE_LEXEMEPARSER_H
