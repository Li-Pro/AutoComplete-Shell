/*
 * Copyrighted (c) 2020 Li-Pro
 */
#include<iostream>
#include<initializer_list>
#include<algorithm>
#include<cassert>
#include<cstring>
#include<cctype>
#include<limits>
#include<thread>
#include<vector>

#include<windows.h>
#include<conio.h>

#include "util/common.hpp"
#include "api/winutil.hpp"
#include "config/config.hpp"

std::vector<std::string> getSuggest(std::string x, std::vector<std::string> &v)
{
	const auto filter = [&](std::string x, std::string pat) -> bool
	{
		// TODO: general filter
		if (x.size() > pat.size()) return false;
		return pat.substr(0, x.size()) == x;
	};
	
	if (!x.size()) return {};
	
	std::vector<std::string> vf;
	
	for (std::string &pat: v)
		if (filter(x, pat)) vf.push_back(pat);
	
	return vf;
}

std::string getLastToken(std::string v)
{
	std::string pattern = GLB_CONF.DELIM;
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
	/* Supported functions:
	 * 		- Arrow L/R: move cursor
	 *		- Arrow U/D: view history
	 * 		- Backspace: pop back
	 *		- Tab: auto-complete(?)
	 *		- PGUP/PGDOWN: switch suggestion
	 */
	
	static std::vector<std::string> shHistory;
	
	const int TAB = 9, BACK = 8, RET = 13;
	
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) exit(1);
	
	DWORD mode;
	GetConsoleMode(hIn, &mode);
	SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	
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
		
		reClean(0); reWrite(0);
		std::string token = getLastToken(input);
		std::vector<std::string> vf = getSuggest(token, suggestion);
		suggest_cnt = std::max(0, std::min((int)vf.size()-1, suggest_cnt));
		
		if (vf.size())
		{
			suggest = vf[suggest_cnt].substr(token.size());
			COORD pos = getCursor();
			
			setTextAttrib(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			setCursor(oCursor); moveCursor(input.size());
			writeStr(suggest);
			resetTextAttrib();
			
			setCursor(pos);
		}
		else suggest = "";
	}
	
	writeStr('\n');
	SetConsoleMode(hIn, mode);
	return input;
}

std::string runShellUtil(std::vector<std::string> pool={}, int flavor=1)
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

std::string runShell(std::initializer_list< std::vector<std::string> > vlist, int flavor=1)
{
	GLB_CONF.DELIM = " ";
	
	typedef std::vector<std::string> vecstr;
	const std::vector< vecstr > vall = vlist;
	vecstr sum;
	for (auto vec: vall)
		for (auto vecx: vec) sum.push_back(vecx);
	return runShellUtil(sum, flavor);
}

std::string runShell(std::vector<std::string> v, int flavor=1)
{
	return runShell({v}, flavor);
}

std::string runShell(int flavor=1)
{
	return runShell({}, flavor);
}

std::string SuperInput(std::initializer_list< std::vector<std::string> > vlist={})
{
	GLB_CONF.DELIM = ", ";
	
	return runShell(vlist, 1);
}

std::string SuperInput(std::vector<std::string> v)
{
	return SuperInput({v});
}
