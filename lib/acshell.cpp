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

#include "flav/shell_base.hpp"
#include "flav/shell1.hpp"

ShellBase *shells[] = { new ShF1() };
//ShellBase shells[] = { ShF1() };
//const std::vector<ShellBase> shells = { ShF1() };

std::string runShellUtil(std::vector<std::string> pool={}, int flavor=1)
{
#if defined(_WIN32) || defined(_WIN64)
//	static /*ShellBase*/ ShF1 shells[] = { ShF1() };
	
	if (pool.size())
	{
		sort(pool.begin(), pool.end());
		pool.erase(std::unique(pool.begin(), pool.end()), pool.end());
	}
	
//	return shells[flavor-1].shell(pool);
	return shells[flavor-1]->shell(pool);
	
#else
	
	#error Unsupported platform.
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
