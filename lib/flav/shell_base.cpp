#include<iostream>
#include<algorithm>
#include<vector>

#include "shell_base.hpp"
#include "../config/config.hpp"

std::string ShellBase::shell(std::vector<std::string> suggestion) { return ""; }
ShellBase::~ShellBase() { }

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
