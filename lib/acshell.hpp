#ifndef ACShell_HPP
#define ACShell_HPP
	
	#include<cstring>
	#include<vector>
	std::string runShell(std::initializer_list< std::vector<std::string> > vlist);
	std::string runShell(std::vector<std::string> v);
	
	std::string SuperInput(std::initializer_list< std::vector<std::string> > vlist={});
	std::string SuperInput(std::vector<std::string> v);
	
#endif
