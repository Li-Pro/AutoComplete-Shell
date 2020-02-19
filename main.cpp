#include<iostream>
#include<windows.h> ////////////
#include<conio.h>

#include "acshell.hpp"

CHAR_INFO dst[5*5];

int main()
{
	int x;
	while ((x=getch())!=26) std::cout<<x<<' '<<(char)x<<'\n';
	
	for (int i=1;i<=8;i++)
	{
		for (int j=8;j>=1;j--)
			std::cout<<j;
		std::cout<<'\n';
	}
	
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut==NULL) return std::cerr<<"Failed retrieving hOut\n", 1;
	
	COORD size = {5, 5};
	SMALL_RECT src = {0, 0, 4, 4}; // LTRB
	
	if (!ReadConsoleOutput(hOut, dst, size, {0, 0}, &src))
	{
		return std::cerr<<"Failed reading console output.\n", 2;
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
	
	using namespace std;
	string v; read(v, {"apple", "auto", "ball", "balloon", "cat"});
	cout<<"Result: "<<v<<'\n';
}
