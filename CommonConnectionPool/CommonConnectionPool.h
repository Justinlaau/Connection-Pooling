#pragma once
/*
* connection pooling function's modules
* 
* the project used lazy load + singleton mode, thread safe
*/
#include"pch.h"
#include<string>
#include<queue>
#include"Connection.h"
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<chrono>
#include<condition_variable>
using namespace std;
class ConnectionPool {
public:
	//get instance of Connection pool
	static ConnectionPool* getConnectionPool();
	//interface for external usage, get a connection from the pool
	//in the previous version, i also created a backConnection(), it is too
	//stupid, use shared_ptr, and overload its destructor to help take back the resource
	shared_ptr<Connection> getConnection();

	//independent thread that responsible for producing new connection
	void produceConnectionTask();
private:
	
	//load the config file
	bool loadConfigFile();
private:
	//one instance, private the constructor
	ConnectionPool();
	//database name
	string _dbname;
	//ip address of database
	string _ip;
	unsigned short _port;
	string _username;
	string _password;

	//initial number of connections of pool 
	int _initSize;
	//max number of connections of pool
	int _maxSize;
	//max idle time of connection
	int _maxIdleTime;
	//the time out of making connection to database 
	int _connectionTimeout;


	//store mysql connnection, multiple thread would access this queue
	queue<Connection*> _connectionQueue;
	//maintain thread safe of _connectionQueue
	mutex _queueMutex;
	//the total number of connection created
	//needs to be atomic or use one more mutex to make sure it is thread safe
	//_connectionCnt++ is not a thread safe operation, but atomic_int type is 
	//thread safe
	atomic_int _connectionCnt;
	//consumer or producer thread communicates using _cond_v
	condition_variable _cond_v;
};