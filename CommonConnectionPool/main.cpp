#include<iostream>
#include"pch.h"
#include"Connection.h"
#include"CommonConnectionPool.h"
#include<memory>
int main()
{
    clock_t start = clock();
    /*
    for (int i = 0; i < 1000; i++) {
        Connection conn;
        char sql[1024] = { 0 };
        sprintf_s(sql, "insert into user(id, name, age, sex) values('%d', '%s', '%d', '%s')",
            0, "justin", 22, "male");
        conn.connect("127.0.0.1", 3306, "root", "1111", "user");
        conn.update(sql);
    }
    */

    for (int i = 0; i < 1000; i++) {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        shared_ptr<Connection> sp =  cp->getConnection();
        char sql[1024] = { 0 };
        sprintf_s(sql, "insert into user(name, age, sex) values('%s', '%d', '%s')",
            "zustin" + char(i), i, "male");
        sp->update(sql);
    }
    clock_t end = clock();
    cout << (end - start) << "ms" << endl;
    return 0;
}