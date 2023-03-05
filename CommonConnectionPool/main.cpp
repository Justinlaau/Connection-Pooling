#include<iostream>
#include"pch.h"
#include"Connection.h"
#include"CommonConnectionPool.h"
#include<memory>
#include<vector>
int main()
{

    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "1111", "user");
    int j = 0;
    while (j++ < 10) {};
    clock_t start = clock();
    auto normal_test = []() {
        for (int i = 0; i < 2500; i++) {
            Connection conn;
            conn.connect("127.0.0.1", 3306, "root", "1111", "user");
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name, age, sex) values('%s', '%d', '%s')",
                "justin", 20, "male");
            conn.update(sql);
        }
    };
    auto connPool_test = []() {
        for (int i = 0; i < 2500; i++) {
            ConnectionPool* cp = ConnectionPool::getConnectionPool();
            shared_ptr<Connection> sp = cp->getConnection();
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name, age, sex) values('%s', '%d', '%s')",
                to_string(i).c_str(), i, "male");
            sp->update(sql);
        }
    };
    int i;
    vector<thread> threads;
    for (i = 0; i < 4; i++) {
        threads.push_back(thread(connPool_test));
    }

    for (i = 0; i < 4; i++) {
        threads[i].join();
    }

    clock_t end = clock();
    cout << (end - start) << "ms" << endl;
    return 0;
}