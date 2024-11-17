//
// Created by Семён on 14.11.2024.
//

#include "LexemeParser.h"
#include "Lexeme.h"
#include "Stream.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

std::vector<Lexeme> LexemeParser::GetLexemes(const std::string &input) {
    std::vector<Lexeme> result;
    std::vector<std::string> resvOperators = {"!", "+", "*", "/", "-", "%",
                                              "=", "&&", "!=", "||", "^^",
                                              "<", "<=", ">=", ">", "|"};
    std::vector<std::string> resvKeywords = {"create", "table", "unique", "autoincrement", "key",
                                             "insert", "to",
                                             "select", "from", "where",
                                             "update", "set",
                                             "delete",
                                             "join", "on",
                                             "update",
                                             "index", "ordered", "unordered",
                                             "int", "string", "bytes", "bool"};

//    auto input = input_query;

    int cur_line = 1;
    int line_start_pos = 0;

    Stream stream(input);
    while (!stream.isEnd()) {
        char cur_rd = stream.peek();
        if (cur_rd == ' ' || cur_rd == '\n' || cur_rd == '\t') {
            stream.seek(1);
//            result.push_back(Lexeme(ELexemeType::Null, cur_line,
//                                    stream.tell() - line_start_pos));
            continue;
        } else if (cur_rd >= '0' && cur_rd <= '9') {
            stream.seek(1);
            if (!stream.isEnd() && stream.peek() == 'x') {
                std::string bytes_seq = "0x";
                stream.seek(1);
                for (int i = 0; !stream.isEnd(); ++i) {
                    if (stream.peek() < '0' || stream.peek() > '9') break;
                    bytes_seq += stream.get();
                }
                result.push_back(Lexeme(ELexemeType::LiteralBytes, bytes_seq,
                                        cur_line, stream.tell() - line_start_pos));
                continue;
            }
            stream.seek(-1);
            auto num = stream.getNum(stream.tell());
            result.push_back(Lexeme(ELexemeType::LiteralNum64, num, cur_line,
                                    stream.tell() - line_start_pos));
            continue;
        } else if (cur_rd == '{' || cur_rd == '}') {
            stream.seek(1);
            result.push_back(Lexeme(ELexemeType::CurlyBrack, cur_rd, cur_line,
                                    stream.tell() - line_start_pos));
            continue;
        }  else if (cur_rd == '[' || cur_rd == ']') {
            stream.seek(1);
            result.push_back(Lexeme(ELexemeType::BoxBrack, cur_rd, cur_line,
                                    stream.tell() - line_start_pos));
            continue;
        }  else if (cur_rd == '(' || cur_rd == ')') {
            stream.seek(1);
            result.push_back(Lexeme(ELexemeType::RoundBrack, cur_rd, cur_line,
                                    stream.tell() - line_start_pos));
            continue;
        }  else if (cur_rd == ',' || cur_rd == ':' || cur_rd == '.') {
            stream.seek(1);
            result.push_back(Lexeme(ELexemeType::Punctuation, cur_rd, cur_line,
                                    stream.tell() - line_start_pos));
            continue;
        } else if (cur_rd == '"' || cur_rd == '\'') {
            stream.seek(1);
            std::string var_name;
            for (int i = 0; !stream.isEnd(); ++i) {
                if (stream.peek() == cur_rd) break;
                var_name += stream.get();
            }
            stream.seek(1);
            result.push_back(Lexeme(ELexemeType::LiteralStr, var_name,
                                    cur_line, stream.tell() - line_start_pos));
            continue;
        } else {
            auto checkIfInList = [&stream](std::vector<std::string> list) {
                auto st_pos = stream.tell();
                std::tuple<bool, std::string> res = {false, ""};
                for (int i = 0; !stream.isEnd() && !list.empty(); ++i) {
                    char chr = stream.get();
                    for (auto it = list.begin(); it < list.end();) {
                        if (it->at(i) != chr) {
                            it = list.erase(it);
                        } else if (it->size() == i + 1) {
                            bool flag = false;
                            if (stream.isEnd()) {
                                flag = true;
                            } else {
                                char c = std::tolower(stream.peek());
                                if (!(c >= 'a' && c <= 'z') &&
                                    !(c >= '0' && c <= '9') && c != '_') {
                                    flag = true;
                                }
                                if (chr == '|' || chr == '-') {
                                    flag = true;
                                }
                            }
                            if (flag) {
                                res = {true, *it};
                            }
                            it = list.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
                stream.setCur(st_pos);
                return res;
            };

            auto [validKeyword, str] = checkIfInList(resvKeywords);
            if (validKeyword) {
                stream.seek(str.size());
                result.push_back(Lexeme(ELexemeType::Keyword, str, cur_line,
                                        stream.tell() - line_start_pos));
                continue;
            }

            auto [validOperator, str2] = checkIfInList(resvOperators);
            if (validOperator) {
                stream.seek(str2.size());
                result.push_back(Lexeme(ELexemeType::Operator, str2, cur_line,
                                        stream.tell() - line_start_pos));
                continue;
            }

            std::string identifier_name;
            for (int i = 0; !stream.isEnd(); ++i) {
                char c = std::tolower(stream.peek());
                if (!(c >= 'a' && c <= 'z') && !(c >= '0' && c <= '9') &&
                    c != '_') {
                    break;
                }
                identifier_name += stream.get();
            }

            if (identifier_name.empty()) {
                result.push_back(Lexeme(ELexemeType::Invalid, cur_line,
                                        stream.tell() - line_start_pos));
                stream.seek(1);
            } else {
                result.push_back(Lexeme(ELexemeType::Identifier,
                                        identifier_name, cur_line,
                                        stream.tell() - line_start_pos));
            }
        }
    }

    result.push_back(Lexeme(ELexemeType::ProgramEnd, cur_line,
                            stream.tell() - line_start_pos));
    return result;
};

