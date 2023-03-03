#pragma once
/*
* implement MySQL operation
*/

#include<iostream>
#include<mysql.h>
#include<string>
#include"public.h"
#include"pch.h"
#include<ctime>
class Connection {
public:
	// init connection
	Connection();

	// release connection resources
	~Connection();

	// connect to database
	bool connect(std::string ip, unsigned short port,
		std::string user, std::string password,
		std::string dbname);

	// insert、delete、update
	bool update(std::string sql);

	// select
	MYSQL_RES* query(std::string sql);

	//refresh idle time of a connection
	void refreshAliveTime() { _alive_time = clock(); }

	//return alive time
	clock_t getAliveTime() { return clock() - _alive_time; }
private:
	MYSQL* _conn; // one connection to MySQL server
	// counting the idle time (when the connection is in the queue)
	clock_t _alive_time=clock();
};
