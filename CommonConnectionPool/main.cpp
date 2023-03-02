#include<iostream>
#include"pch.h"
#include"Connection.h"
#include"CommonConnectionPool.h"
int main()
{
    /* test Connection class
    Connection conn;
    char sql[1024] = {0};
    sprintf_s(sql, "insert into user(id, name, age, sex) values('%d', '%s', '%d', '%s')",
        0, "justin", 22, "male");
    conn.connect("127.0.0.1", 3306, "root", "1111", "user");
    conn.update(sql);
    */
    /* test the configfile
    *ConnectionPool* cp = ConnectionPool::getConnectionPool();
    *cp->loadConfigFile();
    */
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    
    return 0;
}