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
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), &x, 1, &cnt, NULL); // OwO
	SetConsoleMode(hIn, mode);
	return x;
}

TCHAR readNxt()
{
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn==NULL) return 0;
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)); // single char mode
	
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
	
	std::cout<<"##: "<<N<<std::endl;
//	if (!N) Sleep(50);
//	else std::cout<<"##: "<<N<<std::endl;
	
	for (int i=0;i<(int)N;i++)
		if (rec[i].EventType == KEY_EVENT)
		{
			KEY_EVENT_RECORD &krec = rec[i].Event.KeyEvent;
			if (krec.bKeyDown)
			{
				std::cout<<"##sfpeek: "<<(int)krec.uChar.AsciiChar<<std::endl;
				return krec.uChar.AsciiChar;
			}
		}
	
	Error:
	SetConsoleMode(hIn, mode);
	return 0;
}

void shell()
{
	const int TAB = 9, BACK = 8, RET = 13;
	
	// TODO: Error handle
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
	while (true)
	{
		int key=-1;
		
		if (GetKeyState(RET) & (1<<15)) { peekNxt(); break; }
		CONSOLE_SCREEN_BUFFER_INFO scrInfo;
		
		if (sfpeekNxt() == TAB) key = readNxt();
		else
		{
			key = peekNxt();
//			if (sfpeekNxt() > 26) 
			std::cout<<"ESESE\n";
//			else std::cout<<"!!!!!"<<std::endl;
		}
		
		if (key==-1) continue;
		std::cout<<" get: "<<key<<std::endl;
//			key = getch();
		if (!GetConsoleScreenBufferInfo(hOut, &scrInfo))
			raise("Failed reading buffer info", GetLastError());
		
		COORD pos = scrInfo.dwCursorPosition;
		
		int px = pos.X, py = pos.Y;
//		if (key == TAB)
//		{
//			auto tst = "TAB"; DWORD cnt;
//			WriteConsole(hOut, tst, 3, &cnt, NULL);
//		}
	}
//	std::cout<<std::endl;
}

std::string read(std::vector<std::string> pool)
{
//	std::thread doShell(shell);
	
//	auto test = []()
//	{
//		while (true)
//		{
//			if (GetKeyState(13) & (1<<15)) break;
//			if (GetAsyncKeyState(9) & 1) { std::cout<<"OwO"<<std::flush; }
//		}
//	};
//	std::thread tst(test);
	
	// TODO: KeyState to peek / read
//	while (peekNxt()!=13);
	shell();
	
//	tst.join();
	
	const auto filter = [&](std::string x, std::string pat) -> bool
	{
		// TODO: general filter
		if (pat.size() < x.size()) return false;
		return pat.substr(x.size()) == x;
	};
	filter("a", "abc"); // true
	filter("ac", "abc"); // false
	
	for (int i=1;i<=8;i++)
	{
		for (int j=8;j>=1;j--)
			std::cout<<j;
		std::cout<<'\n';
	}
	
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut==NULL) raise("Failed retrieving hOut\n", 1);
	
	COORD size = {5, 5};
	SMALL_RECT src = {0, 0, 4, 4}; // LTRB
	
	if (!ReadConsoleOutput(hOut, dst, size, {0, 0}, &src))
	{
		raise("Failed reading console output.\n", 2);
	}
	else
	{
		std::cout<<"Read: \n";
		for (int i=0;i<size.X;i++)
		{
			for (int j=0;j<size.Y;j++)
				std::cout<<dst[i*5+j].Char.AsciiChar;
			std::cout<<'\n';
		}
	}
	
	std::string v;
	std::cin>>v;
	return v;
}
