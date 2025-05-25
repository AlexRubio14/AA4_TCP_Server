#pragma once
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "LoginResult.h"
#include "RegisterResult.h"

#define SERVER "127.0.0.1:3306"
#define USERNAME "root"
#define PASSWORD "enti"
#define DATABASE "parchis_tcp"

#define DB_MANAGER DatabaseManager::Instance()

class DatabaseManager
{
private:

	sql::Driver* driver;
	sql::Connection* con;

	DatabaseManager() = default;
	DatabaseManager(const DatabaseManager&) = delete;
	DatabaseManager& operator=(const DatabaseManager&) = delete;

public:
	static DatabaseManager& Instance();
	void ConnectDb();
	void DisconnectDb();
	RegisterResult CreateUser(const std::string& username, const std::string& password);
	LoginResult ValidateUser(const std::string& username, const std::string& password);

	std::string GenerateHashedPassword(const std::string password);
};

