#pragma once
/*
* implement MySQL operation
*/

#include<iostream>
#include<mysql.h>
#include<string>
#include"public.h"
#include"pch.h"

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
private:
	MYSQL* _conn; // one connection to MySQL server
};
