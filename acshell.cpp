/*
 * Copyrighted (c) 2020 Li-Pro
 */
#include<iostream>
#include<cassert>
#include<cctype>
#include<thread>
#include<vector>

#include<windows.h>
#include<conio.h>

CHAR_INFO dst[5*5];

void raise(std::string errMsg, int exitCode)
{
	std::cerr<<errMsg; exit(exitCode);
}

TCHAR peekNxt()
{
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn==NULL) return 0;
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)); // single char mode
	
	TCHAR x = 0; DWORD cnt;
	ReadConsole(hIn, &x, 1, &cnt, NULL);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), &x, 1, &cnt, NULL);
	SetConsoleMode(hIn, mode);
	return x;
}

TCHAR readNxt()
{
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn==NULL) return 0;
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	
	TCHAR x = 0; DWORD cnt;
	ReadConsole(hIn, &x, 1, &cnt, NULL);
	SetConsoleMode(hIn, mode);
	return x;
}

TCHAR sfpeekNxt()
{
	static INPUT_RECORD rec[1024]; //// length enough?
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn == NULL) return 0;
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	
	DWORD N;
	if (!PeekConsoleInput(hIn, rec, 1024, &N)) goto Error;
	
	if (N) std::cout<<"##: "<<N<<std::endl;
	
	for (int i=0;i<(int)N;i++)
		if (rec[i].EventType == KEY_EVENT)
		{
			KEY_EVENT_RECORD &krec = rec[i].Event.KeyEvent;
			if (krec.bKeyDown)
			{
//				std::cout<<"##sfpeek: "<<(int)krec.uChar.AsciiChar<<std::endl;
				return krec.uChar.AsciiChar;
			}
		}
	
	Error:
	SetConsoleMode(hIn, mode);
	return 0;
}

bool shell_finish;
void shell()
{
	const int TAB = 9, BACK = 8, RET = 13;
	
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) exit(1);
	
//	const auto writeAt = [&](std::string v, int x, int y) -> unsigned long
//	{
//		unsigned long rep = 0;
//		WriteConsoleOutputCharacter(hOut, v.c_str(), v.size(), {(short)x, (short)y}, &rep);
//		return rep;
//	};
	
	assert(sizeof(short) == 2); // TODO
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	
	int typed=0;
	std::vector<char> input;
	while (!shell_finish)
	{
//		std::cout<<"#####"<<std::endl;
//		MSG msg;
//		int rep = -1;
//		while ((rep = GetMessage(&msg, NULL, 0, 0)) != 0)
//		{
//			std::cout<<"###"<<std::endl;
//			if (rep == -1) raise("GetMessage Failed.", GetLastError());
//			
//			if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
//			{
//				std::cout<<"#####"<<std::endl;
//			}
//			else
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//		}
		if (GetAsyncKeyState(TAB) & (1<<15)) std::cout<<(char)BACK;
	}
//	std::cout<<"#"<<std::endl;
	
	SetConsoleMode(hIn, mode);
}

std::string read(std::vector<std::string> pool)
{
#if defined(_WIN32) || defined(_WIN64)
//	shell();
	
	const auto filter = [&](std::string x, std::string pat) -> bool
	{
		// TODO: general filter
		if (pat.size() < x.size()) return false;
		return pat.substr(x.size()) == x;
	};
	filter("a", "abc"); // true
	filter("ac", "abc"); // false
	
//	int key=-1;
//	while ((key=getch()))
//	{
//		if (key=='a') std::cout<<(char)8;
//		else std::cout<<(char)key;
//	}
	
	std::thread tshell(shell);
	std::string v;
	std::cin>>v;
	shell_finish = 1, tshell.join();
//	shell();
	return v;
	
#else
	raise("Unsupoorted platform.\n", 1);
	return {};
#endif
}
