#ifndef ACShell_API_WINUTIL_HPP
#define ACShell_API_WINUTIL_HPP

#if defined(_WIN32) || defined(_WIN64)

	#include<iostream>
	#include<cstring>
	
	#include<windows.h>
	
	TCHAR peekNxt();
	TCHAR readNxt();
	
	int writeStr(std::string v);
	int writeStr(char x);
	int writeStay(std::string v);
	int writeStay(char x);
	
	COORD getCursor();
	bool setCursor(short x, short y);
	bool setCursor(COORD pos);
	bool moveCursor(short vx, short vy);
	bool moveCursor(int dis);
	
	int disFrom(COORD src, COORD at);
	std::string readStr(short dis);
	
	void setTextAttrib(int attrib);
	void resetTextAttrib();

#endif

#endif
