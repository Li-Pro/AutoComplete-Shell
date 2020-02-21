#include<iostream>

#include "lib/acshell.hpp" // main library

int main()
{
	using namespace std;
	string v = read({"apple", "auto", "angry", "ball", "balloon", "cat", "cool", "do", "does", "elephant"});
	cout<<"Result: "<<v<<'\n';
}
