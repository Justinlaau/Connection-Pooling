#include "CommonConnectionPool.h"
#include "pch.h"
#include "public.h"

ConnectionPool* ConnectionPool::getConnectionPool() {
/*thread safe lazy loading singleton mode, the whole programe only has one instance
* it is thread safe, because we use static to create an instance,
* complier would help us to use lock and unlock when handling static init
*/
	static ConnectionPool pool;
	return &pool;
}

ConnectionPool::ConnectionPool() {
	//load config
	if (!loadConfigFile()) {
		return;
	}
	//create initSize connection
	//no need to consider thread safe problem here
	//as programe just boot up, no other threads going into this area

	for (int i = 0; i < _initSize; i++) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		//store the current time point
		p->refreshAliveTime();
		_connectionQueue.push(p);
		_connectionCnt++;
	}
	// create a thread produce connection
	// we have to use bind here, to bind the this pointer to the function
	// becuase thread is C interface, does not support this
	// i used to write a out class function and receive a object to do so
	// but this is way more convient to access the data and smart
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	//let it run in background, deamon thread
	produce.detach();
	// create a thread to loop the connectionQueue to check the idle connection
	// if exceed the max idle time, break it
	thread scanner(std::bind(&ConnectionPool::scanConnectionTask, this));
	//let it run in background, deamon thread
	scanner.detach();
}	

bool ConnectionPool::loadConfigFile() {

	FILE* pf = fopen("mysql.ini", "r");

	if (pf == nullptr) {
		LOG("mysql.ini file is not exist!");
		return 0;
	}

	while (!feof(pf)) {
		//if the config file's line character > 1023, would crash
		char line[1024]{ 0 };
		fgets(line, 1024, pf);
		string str(line);
		//config file format -> key=value
		int idx = str.find('=', 0);
		if (-1 == idx) {
			//invalid config setting, load next item
			continue;
		}
		//password=1111\n
		int end_idx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, end_idx - idx - 1);
		if (key == "ip") {
			_ip = value;
		}
		else if (key == "port") {
			_port = atoi(value.c_str());
		}
		else if (key == "username") {
			_username = value;
		}
		else if (key == "password") {
			_password = value;
		}
		else if (key == "initSize") {
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize") {
			_maxSize = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut") {
			_connectionTimeout = atoi(value.c_str());
		}
		else if (key == "dbname") {
			_dbname = value;
		}
		else if (key == "maxIdleTime") {
			_maxIdleTime = atoi(value.c_str());
		}
	}
	return 1;
}

void ConnectionPool::produceConnectionTask() {
	for (;;) {
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQueue.empty()) {
			//wait to produce, consumer would tell through the _cond_v
			_cond_v.wait(lock);
		}

		if (_connectionCnt < _maxSize) {
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();
			_connectionQueue.push(p);
			_connectionCnt++;
		}
		//tell consumer, connection avaliable
		_cond_v.notify_all();
	}
}

shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	//if not any connection in the queue, just wait for production's notification
	while (_connectionQueue.empty()) {
		// cv_status enum{timeout, no_timeout} timeout is the real timeout
		if (cv_status::timeout == _cond_v.wait_for(lock, chrono::milliseconds(_connectionTimeout))) {

			if (_connectionQueue.empty()) {
				LOG("Connection timeout....cannot connect");
				return nullptr;
			}
			
		}
	}

	auto reload_shared_ptr_deleter = [&](Connection* pcon) {
		//would be called in server application threads, need to condsider thread safety 
		unique_lock<mutex> lock(_queueMutex);
		pcon->refreshAliveTime();
		_connectionQueue.push(pcon);

	};
	shared_ptr<Connection> sp(_connectionQueue.front(), reload_shared_ptr_deleter);
	_connectionQueue.pop();
	//tell producer that i used a connection, production thread is going to check the queue, if empty, produce
	_cond_v.notify_all();

	return sp;
}
#include<windows.h>  
void ConnectionPool::scanConnectionTask() {
	for (;;) {
		// use sleep to simulate timing
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		//scan connectionQueue, release unnessery connections
		unique_lock<mutex> lock(_queueMutex);
		//if _connectionCnt > initSize which means we created additional connection, we release them
		//keep the _initSize of connections in the Pool
		while (_connectionCnt > _initSize) {
			Connection* p = _connectionQueue.front();
			//if alivetime > maxIdleTime, which means the programme doesnt use this connection for a while
			if (p->getAliveTime() >= (_maxIdleTime * 1000)) {
				_connectionQueue.pop();
				_connectionCnt--;
				delete p;
			}
			else {
				//Queue is FIFO, if the first one didnt exceed the time, later one is impossible so
				break;
			}
		}
	}
}