#include<iostream>

#include "lib/acshell.hpp" // main library

int main()
{
	using namespace std;
	vector<string> list_a = {"apple", "auto", "angry", "ball", "balloon"};
	vector<string> list_b = {"cat", "cool", "do", "does", "elephant"};
	
	string v = runShell({list_a, list_b, list_a}, 1);
	cout<<"Result: "<<v<<'\n';
}
