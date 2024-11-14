//
// Created by Семён on 12.11.2024.
//

#include <iostream>
#include "memdb-lib.h"


int main() {
    memdb::Database db;

    db.execute("select posts.id, users.login, posts.text from users join posts on users.id = posts.user_id where true");
    return 0;
}