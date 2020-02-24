#ifndef ACShell_FLAV_SHELL_BASE_HPP
#define ACShell_FLAV_SHELL_BASE_HPP

#include<iostream>
#include<vector>

struct ShellBase
{
	virtual std::string shell(std::vector<std::string> suggestion={});
//	virtual std::string shell(std::vector<std::string> suggestion={}) { return ""; }
	virtual ~ShellBase();
//	virtual ~ShellBase() { }
};

std::vector<std::string> getSuggest(std::string x, std::vector<std::string> &v);
std::string getLastToken(std::string v);

#endif
