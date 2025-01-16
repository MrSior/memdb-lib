//
// Created by Семён on 14.11.2024.
//

#include "Stream.h"

Stream::Stream(std::string str) { str_ = str; }

char Stream::get() { return str_[off_++]; }

char Stream::peek() { return str_[off_]; }

bool Stream::isEnd() { return off_ == str_.size(); }

int Stream::getSize() { return str_.size(); }

int Stream::tell() { return off_; }

void Stream::seek(int diff) { off_ += diff; }

void Stream::setCur(int pos) { off_ = pos; }

int64_t Stream::getNum(int st, bool is_read) {
    int64_t res = 0;
    int64_t deg = 0;
    for (int ind = st; ind < str_.size(); ++ind) {
        if (str_[ind] >= '0' && str_[ind] <= '9') {
            deg = std::max((int64_t)1, deg * 10);
        } else {
            break;
        }
    }

    for (int ind = st; deg; ++ind, deg /= 10) {
        res += (str_[ind] - '0') * deg;
        if (is_read) {
            ++off_;
        }
    }

    return res;
}