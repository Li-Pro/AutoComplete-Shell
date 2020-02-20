#include<iostream>
#include<windows.h>
#include<conio.h>
#include<cctype>
#include<vector>

CHAR_INFO dst[5*5];

void raise(std::string errMsg, int exitCode) { std::cerr<<errMsg; exit(exitCode); }

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
	
	int key=-1, typed=0;
	while ((key=getch()) != RET)
	{
		CONSOLE_SCREEN_BUFFER_INFO scrInfo;
		
		if (!GetConsoleScreenBufferInfo(hOut, &scrInfo))
		{
			std::cout<<GetLastError()<<std::endl;
		}
		
		COORD pos = scrInfo.dwCursorPosition;
		
		int px = pos.X, py = pos.Y;
		if (key==BACK)
		{
			if (typed <= 0) continue;
			writeAt(" ", px-1, py);
			SetConsoleCursorPosition(hOut, {(short)(px-1), (short)py});
			typed--;
		}
		else if (key==TAB)
		{
			
		}
		else if (isprint(key))
		{
			writeAt(std::string()+(char)key, px, py);
			SetConsoleCursorPosition(hOut, {(short)(px+1), (short)py});
			typed++;
		}
		else
		{
			if (key==0xE0)
			{
				key = getch();
			}
		}
		// TODO: key > 26
	}
	
	if (key==RET) std::cout<<'\n';
}

void read(std::string &v, std::vector<std::string> pool)
{
	shell();
	
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
	
	std::cin>>v;
}
