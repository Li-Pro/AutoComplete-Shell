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

//TCHAR sfpeekNxt()
//{
//	static INPUT_RECORD rec[1024]; //// length enough?
//	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
//	if (hIn == NULL) return 0;
//	
//	DWORD mode;
//	GetConsoleMode(hIn, &mode);
//	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
//	
//	DWORD N;
//	if (!PeekConsoleInput(hIn, rec, 1024, &N)) goto Error;
//	
//	if (N) std::cout<<"##: "<<N<<std::endl;
//	
//	for (int i=0;i<(int)N;i++)
//		if (rec[i].EventType == KEY_EVENT)
//		{
//			KEY_EVENT_RECORD &krec = rec[i].Event.KeyEvent;
//			if (krec.bKeyDown)
//			{
////				std::cout<<"##sfpeek: "<<(int)krec.uChar.AsciiChar<<std::endl;
//				return krec.uChar.AsciiChar;
//			}
//		}
//	
//	Error:
//	SetConsoleMode(hIn, mode);
//	return 0;
//}

int writeStr(std::string v)
{
	DWORD rep;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), v.c_str(), v.size(), &rep, NULL);
	return rep;
}

int writeStr(char x)
{
	return writeStr(std::string()+x);
}

COORD getCursor()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_INPUT_HANDLE), &info);
	return info.dwCursorPosition;
}

bool setCursor(short x, short y)
{
	return SetCursorPos(x, y);
}

bool setCursor(COORD pos)
{
	return setCursor(pos.X, pos.Y);
}

bool moveCursor(short vx, short vy)
{
	COORD pos = getCursor();
	return setCursor(pos.X+vx, pos.Y+vy);
}

bool moveCursor(short dis)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_INPUT_HANDLE), &info);
	
	COORD pos = getCursor();
	int /*H = info.dwSize.Y, */W = info.dwSize.X;
	
	pos.X -= dis;
	if (pos.X<0)
	{
		int cnt = -pos.X/W + (pos.X%W!=0);
		pos.X += W*cnt, pos.Y -= cnt;
	}
	else if (pos.X>W)
	{
		int cnt = pos.X/W;
		pos.X -= W*cnt, pos.Y += cnt;
	}
	
	return setCursor(pos);
}

int writeStay(std::string v)
{
	COORD pos = getCursor();
	
	int rep = writeStr(v);
	setCursor(pos);
	return rep;
}

int writeStay(char x)
{
	return writeStay(std::string()+x);
}

bool shell_finish;
std::string shell()
{
	const int TAB = 9, BACK = 8, RET = 13;
	
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) exit(1);
	
	assert(sizeof(short) == 2); // TODO
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	
	/* Supported functions:
	 * 		- Arrow L/R: move cursor
	 *		- Arrow U/D: view history
	 * 		- Backspace: pop back
	 *		- Tab: auto-complete(?)
	 */
	
	std::string input;
	while (!shell_finish)
	{
		int key = getch();
		if (key == RET) break;
		else if (isprint(key) || key==' ')
		{
			writeStr(key);
			input.push_back((char)key);
		}
		else if (key == BACK)
		{
			if (!input.size()) continue;
			
			std::cout<<moveCursor(-1)<<'\n';
			writeStay(' ');
			input.pop_back();
		}
		else if (key == TAB)
		{
			
		}
		else if (key == 0xE0)
		{
			int func = getch();
			const int UP=72, LEFT=75, RIGHT=77, DOWN=80;
//			if (func==UP) std::cout<<"UP\n";
//			else if (func==LEFT) std::cout<<"LEFT\n";
//			else if (func==RIGHT) std::cout<<"RIGHT\n";
//			else if (func==DOWN) std::cout<<"DOWN\n";
//			else std::cout<<func<<std::endl;
			
			if (func==LEFT || func==RIGHT)
			{
				
			}
			else if (func==UP || func==DOWN)
			{
				
			}
		}
	}
	
	SetConsoleMode(hIn, mode);
	return input;
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
//	std::string v;
//	std::cin>>v;
//	shell_finish = 1, tshell.join();
//	shell();
//	return v;
	return shell();
	
#else
	raise("Unsupoorted platform.\n", 1);
	return {};
#endif
}
