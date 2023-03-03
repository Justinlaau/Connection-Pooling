#include"Connection.h"

Connection::Connection(){
	// init connection 
	_conn = mysql_init(nullptr);
}

Connection::~Connection(){
	// release connection resources
	if (_conn != nullptr)mysql_close(_conn);
}

bool Connection::connect(std::string ip, unsigned short port, 
						 std::string user, std::string password,
						 std::string dbname){
	// connect to database, mysql_real_connect receive C style char *, c_str() help to convert it
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}

bool Connection::update(std::string sql){
	// insert、delete、update
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("update failed:" + sql);
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(std::string sql){
	// select
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("query failed: " + sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}