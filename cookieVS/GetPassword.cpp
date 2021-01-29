#include "GetPassword.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	int answer = MessageBoxA(NULL, "Hello, nice to meet you!\nDo you have chrome on your computer?", "Test program on windows", MB_YESNO);
	if (answer == IDNO)
	{
		MessageBoxA(NULL, "Why don't you have Chrome? Use Chrome dude!", "Really??", MB_OK);
	}
	else
	{
		answer = MessageBoxA(NULL, "Now, I'm gonna steal 3 of your saved passwords. OK?", "Permission to steal", MB_YESNO);
		if (answer == IDNO)
		{
			MessageBoxA(NULL, "Oh come on! They\'re just passwords.", ":FeelsSadMan:", MB_OK);
			exit(0);
		}
		try
		{
			Dumper::dumpPassword();
		}
		catch (exception& e)
		{
			MessageBoxA(NULL, "Something is wrong! Did you lie to me???", "Liar", MB_OK);
		}
	}

	return 0;
}