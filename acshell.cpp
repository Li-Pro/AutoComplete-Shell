/*
 * Copyrighted (c) 2020 Li-Pro
 */
#include<iostream>
#include<windows.h>
#include<conio.h>
#include<thread>
#include<cctype>
#include<vector>

CHAR_INFO dst[5*5];

void raise(std::string errMsg, int exitCode)
{
	std::cerr<<errMsg; exit(exitCode);
}

void func1()
{
	for (int i=0;i<7;i++)
		std::cout<<"1: "<<i<<std::endl, Sleep(123);
}

void func2()
{
	for (int i=0;i<7;i++)
		std::cout<<"2: "<<i<<std::endl, Sleep(123);
}

LRESULT CALLBACK kbHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	std::cout<<"HOOK: "<<nCode<<std::endl;
	while (GetMessage(NULL, NULL, 0, 0));
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool setupHook(HHOOK &x, int type, HINSTANCE hins=NULL)
{
	return (x = SetWindowsHookExA(type, kbHook, hins, 0)) != NULL;
}

void releaseHook(HHOOK &x)
{
	UnhookWindowsHookEx(x);
}

bool hook_stop;
void initHook()
{
	HHOOK hook;
	if (!setupHook(hook, WH_KEYBOARD_LL, GetModuleHandle(NULL)))
		raise("Failed setting up hook.", GetLastError());
	
	while (!hook_stop);
	
	releaseHook(hook);
}

void shell()
{
//	std::thread t1(func1);
//	std::thread t2(func2);
	
//	t1.join();  t2.join();
//	HINSTANCE hins = GetModuleHandle(NULL);
//	if (hins==NULL) raise("", GetLastError());
	std::thread thook(initHook);
	
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
	while (true)
	{
		if (kbhit())
		{
			if (GetKeyState(RET) & (1<<15)) break; ////////////// Mind for short's length!
			CONSOLE_SCREEN_BUFFER_INFO scrInfo;
			
			key = getch();
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
	}
	std::cout<<std::endl;
}

std::string read(std::vector<std::string> pool)
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
	
	std::string v;
	std::cin>>v;
	return v;
}
