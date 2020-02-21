/*
 * Copyrighted (c) 2020 Li-Pro
 */
#include<iostream>
#include<initializer_list>
#include<algorithm>
#include<cassert>
#include<cstring>
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
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return info.dwCursorPosition;
}

bool setCursor(short x, short y)
{
	return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x, y});
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

bool moveCursor(short dis) // TODO: short^2 -> int
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	
	COORD pos = getCursor();
	int W = info.dwSize.X;
	
	pos.X += dis;
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

int disFrom(COORD src, COORD at)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	
	int W = info.dwSize.X;
	return (at.Y-src.Y)*W + (at.X-src.X);
}

std::string readStr(short dis)
{
	COORD at = getCursor();
	
	std::string sum;
	for (int i=0;i<dis;i++)
	{
		char x = 0;
		short px = getCursor().X, py = getCursor().Y;
		DWORD cnt = 0;
		ReadConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), &x, 1, {px, py}, &cnt);
		
		sum += x;
		moveCursor(1);
	}
	
	setCursor(at);
	return sum;
}

std::string getSuggest(std::string x, std::vector<std::string> &v, int &cnt)
{
	const auto filter = [&](std::string x, std::string pat) -> bool
	{
		// TODO: general filter
		if (x.size() > pat.size()) return false;
		return pat.substr(0, x.size()) == x;
	};
	
	if (!x.size()) return "";
	
	std::vector<std::string> vf;
	
	for (std::string &pat: v)
		if (filter(x, pat)) vf.push_back(pat);
	
	if (vf.empty()) return "";
	
	cnt = std::max(0, std::min((int)vf.size()-1, cnt));
	return vf[cnt].substr(x.size());
}

void setTextAttrib(int attrib)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attrib);
}

void resetTextAttrib()
{
	setTextAttrib(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

std::string getLastToken(std::string v)
{
	std::string pattern = ", ";
	std::string sum;
	
	std::reverse(v.begin(), v.end());
	for (char x: v)
	{
		if (std::count(pattern.begin(), pattern.end(), x)) break;
		else sum.push_back(x);
	}
	
	std::reverse(sum.begin(), sum.end());
	return sum;
}

std::string shell(std::vector<std::string> suggestion={})
{
	static std::vector<std::string> shHistory;
	
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
	 *		- PGUP/PGDOWN: switch suggestion
	 */
	
	COORD oCursor = getCursor();
	
	shHistory.push_back({});
	int histcnt = shHistory.size(), at = histcnt-1;
	
	std::string input = shHistory[at];
	std::string suggest;
	
	const auto reClean = [&](bool reset=1)
	{
		COORD pos = getCursor();
		
		int len = input.size() + suggest.size();
		setCursor(oCursor);
		writeStay(std::string(len, ' '));
		
		if (reset) setCursor(pos);
	};
	
	const auto reWrite = [&](bool reset=1)
	{
		COORD pos = getCursor();
		
		setCursor(oCursor);
		writeStr(input);
		
		if (reset) setCursor(pos);
	};
	
	int suggest_cnt = 0;
	while (true)
	{
		int key = getch();
		if (key == RET) break;
		else if (isprint(key) || key==' ')
		{
			int inpi = disFrom(oCursor, getCursor());
			
			reClean(1);
			input.insert(inpi, 1, (char)key);
			moveCursor(1);
			
			at = histcnt-1, shHistory.back() = input;
			suggest_cnt = 0;
		}
		else if (key == BACK)
		{
			int inpi = disFrom(oCursor, getCursor());
			if (!inpi) continue;
			else inpi--;
			
			reClean(1);
			input.erase(input.begin()+inpi);
			moveCursor(-1);
			
			at = histcnt-1, shHistory.back() = input;
			suggest_cnt = 0;
		}
		else if (key == TAB)
		{
			if (suggest.empty()) continue;
			
			/* input = input + suggest + ' '; */
			input = input + suggest;
			suggest_cnt = 0;
		}
		else if (key == 0xE0)
		{
			int func = getch();
			const int UP=72, LEFT=75, RIGHT=77, DOWN=80, PGUP=73, PGDOWN=81;
			
			if (func==LEFT || func==RIGHT)
			{
				if (func==LEFT && disFrom(oCursor, getCursor())>0) moveCursor(-1);
				if (func==RIGHT && disFrom(oCursor, getCursor())<(int)input.size()) moveCursor(1);
			}
			else if (func==UP || func==DOWN)
			{
				setCursor(oCursor);
				writeStay(std::string(input.size(), ' '));
				
				at += (func==UP? -1: 1);
				at = std::max(0, std::min(histcnt-1, at));
				
				input = shHistory[at];
				writeStr(input);
			}
			else if (func==PGUP||func==PGDOWN)
			{
				suggest_cnt += (func==PGUP? -1: 1);
			}
		}
		else if (key==3) break;
		
		reClean(0);
		suggest = getSuggest(getLastToken(input), suggestion, suggest_cnt);
		reWrite(0);
		if (suggest.size())
		{
			COORD pos = getCursor();
			
			setTextAttrib(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			setCursor(oCursor); moveCursor(input.size());
			writeStr(suggest);
			resetTextAttrib();
			
			setCursor(pos);
		}
	}
	
	writeStr('\n');
	SetConsoleMode(hIn, mode);
	return input;
}

std::string runShellUtil(std::vector<std::string> pool={})
{
#if defined(_WIN32) || defined(_WIN64)
	if (pool.size())
	{
		sort(pool.begin(), pool.end());
		pool.erase(std::unique(pool.begin(), pool.end()), pool.end());
	}
	
	return shell(pool);
	
#else
	raise("Unsupoorted platform.\n", 1);
	return {};
#endif
}

//template<typename T>
//std::vector<T> mergeVector(std::vector<T> v1, std::vector<T> v2)
//{
//	std::vector<T> sum;
//	for (T x: v1) sum.push_back(x);
//	for (T x: v2) sum.push_back(x);
//	return sum;
//}
//namespace
//{
std::string runShell(std::initializer_list< std::vector<std::string> > vlist={})
{
	typedef std::vector<std::string> vecstr;
	const std::vector< vecstr > vall = vlist;
	vecstr sum;
	for (auto vec: vall)
		for (auto vecx: vec) sum.push_back(vecx);
	return runShellUtil(sum);
}

std::string runShell(std::vector<std::string> v)
{
	return runShell({v});
}
//}

//template<typename... T2>
//std::string SuperInput(T2... pool)
std::string SuperInput(std::initializer_list< std::vector<std::string> > vlist={})
{
//	return runShell(pool...);
	return runShell(vlist);
}

std::string SuperInput(std::vector<std::string> v)
{
	return SuperInput({v});
}
