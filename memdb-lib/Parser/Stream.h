//
// Created by Семён on 14.11.2024.
//

#ifndef DATABASE_STREAM_H
#define DATABASE_STREAM_H

#include <cstddef>
#include <cstdint>
#include <string>

class Stream {
public:
    char get();
    char peek();
    void seek(int diff);
    void setCur(int pos);
    int tell();
    bool isEnd();
    int getSize();
    Stream(std::string str = "");

    int64_t getNum(int st, bool is_read = true);

private:
    std::string str_ = "";
    int off_ = 0;
};


#endif //DATABASE_STREAM_H
