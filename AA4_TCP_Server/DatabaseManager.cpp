#include "DatabaseManager.h"
#include "bcrypt/include/bcrypt.h"
#include <cassert>
#include "ClientManager.h"

DatabaseManager& DatabaseManager::Instance()
{
    static DatabaseManager instance;
    return instance;
}

void DatabaseManager::ConnectDb()
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(SERVER, USERNAME, PASSWORD);
        con->setSchema(DATABASE);
        std::cout << "Connection with Db DONE" << std::endl;
    }
    catch (sql::SQLException e)
    {
        std::cerr << "Connection with Db failed because: " << e.what() << std::endl;
    }
}

void DatabaseManager::DisconnectDb()
{
    con->close();

    if (con->isClosed())
    {
        std::cout << "Connection close" << std::endl;
        delete con;
    }
}

RegisterResult DatabaseManager::CreateUser(const std::string& username, const std::string& password)
{
    try
    {
		// Check if the username already exists
		std::string checkQuery = "SELECT id FROM users WHERE username = ?";
        sql::PreparedStatement* checkStat = con->prepareStatement(checkQuery);
        checkStat->setString(1, username);
		sql::ResultSet* checkResult = checkStat->executeQuery();

        if (checkResult->next())
        {
			std::cout << "Username: " << username << " already taken" << std::endl;
			delete checkResult;
			delete checkStat;
			return RegisterResult::USERNAME_TAKEN;
        }


        std::string insertQuery = "INSERT INTO users (username, password) VALUES (?, ?)";

        std::string hashedPassword = GenerateHashedPassword(password);
		sql::PreparedStatement* insertStat = con->prepareStatement(insertQuery);

        insertStat->setString(1, username);
        insertStat->setString(2, hashedPassword);

        int affected_rows = insertStat->executeUpdate();
        delete insertStat;

        if (affected_rows > 0)
        {
            std::cout << "User registered in db: " << username << std::endl;
            return RegisterResult::SUCCESS;
        }
        else
        {
			std::cout << "An error ocurred while inserting user: " << username << std::endl;
            return RegisterResult::INSERT_FAILED;
        }
    }
    catch (sql::SQLException error)
    {
        std::cerr << "Error creating user because: " << error.what() << std::endl;
		return RegisterResult::QUERY_ERROR;
    }
}

LoginResult DatabaseManager::ValidateUser(const std::string& username, const std::string& password)
{
	try
	{
		std::string query = "SELECT password FROM users WHERE username = ?";
		sql::PreparedStatement* stat = con->prepareStatement(query);

		stat->setString(1, username);
		sql::ResultSet* result = stat->executeQuery();

		// Check if the user exists
		if (result->next())
		{
			std::string hashedPassword = result->getString("password");

			// CHeck if the password is correct
			if (bcrypt::validatePassword(password, hashedPassword))
			{
                // Check if user is already logged
                if (CLIENT_MANAGER.CheckIfUserAlreadyLogged(username))
                    return LoginResult::USER_ALREADY_LOGGED;

				std::cout << "User validated" << std::endl;
				return LoginResult::SUCCESS;
			}

			std::cout << "Password not correct" << std::endl;
		}

		delete result;
		delete stat;

		std::cout << "Username not found" << std::endl;
		return LoginResult::INVALID_CREDENTIALS;
	}
	catch (sql::SQLException error)
	{
		std::cerr << "Error validating user because: " << error.what() << std::endl;
        return LoginResult::QUERY_ERROR;
	}
}

std::string DatabaseManager::GenerateHashedPassword(const std::string password)
{
    unsigned int hashRound = 12;

	std::string hash = bcrypt::generateHash(password, hashRound);

    return hash;
}

