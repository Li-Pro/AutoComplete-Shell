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
	
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) exit(1);
	
	const auto writeAt = [&](std::string v, int x, int y) -> unsigned long
	{
		unsigned long rep = 0;
		WriteConsoleOutputCharacter(hOut, v.c_str(), v.size(), {(short)x, (short)y}, &rep);
		return rep;
	};
	
	assert(sizeof(short) == 2); // TODO
	
	int typed=0;
	while (!shell_finish)
	{
		int key=-1;
		
		if (GetKeyState(RET) & (1<<15)) { peekNxt(); break; }
		CONSOLE_SCREEN_BUFFER_INFO scrInfo;
//		if (GetKeyState(TAB) & (1<<15)) { /*std::cout<<"########"<<std::endl;*/ key = readNxt(); }
		
		while (!(key = sfpeekNxt()));
		
//		if (sfpeekNxt() == TAB) key = readNxt();
//		else
//		{
//			if (sfpeekNxt() > 26) 
//			key = peekNxt();
//			std::cout<<"ESESE\n";
//		}
		
//		if (key==-1) continue;
		if (key > 26)
		{
			assert(key == peekNxt());
//			key = readNxt();
			//std::cout<<" get: "<<key<<std::endl;
		}
		else if (key == BACK) peekNxt();
		else std::cout<<"####\n", readNxt();
		
//		if (!GetConsoleScreenBufferInfo(hOut, &scrInfo))
//			raise("Failed reading buffer info", GetLastError());
//		
//		COORD pos = scrInfo.dwCursorPosition;
//		
//		int px = pos.X, py = pos.Y;
	}
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
	
//	std::thread tshell(shell);
	std::string v;
//	std::cin>>v;
//	shell_finish = 1, tshell.join();
	shell();
	return v;
	
//	for (int i=1;i<=8;i++)
//	{
//		for (int j=8;j>=1;j--)
//			std::cout<<j;
//		std::cout<<'\n';
//	}
//	
//	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//	if (hOut==NULL) raise("Failed retrieving hOut\n", GetLastError());
//	
//	COORD size = {5, 5};
//	SMALL_RECT src = {0, 0, 4, 4}; // LTRB
//	
//	if (!ReadConsoleOutput(hOut, dst, size, {0, 0}, &src))
//	{
//		raise("Failed reading console output.\n", GetLastError());
//	}
//	else
//	{
//		std::cout<<"Read: \n";
//		for (int i=0;i<size.X;i++)
//		{
//			for (int j=0;j<size.Y;j++)
//				std::cout<<dst[i*5+j].Char.AsciiChar;
//			std::cout<<'\n';
//		}
//	}
	
//	std::string v;
//	std::cin>>v;
//	return v;
#else
	raise("Unsupoorted platform.\n", 1);
	return {};
#endif
}
