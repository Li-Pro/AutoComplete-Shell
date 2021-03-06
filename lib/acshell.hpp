#ifndef ACShell_HPP
#define ACShell_HPP
	
	#include<cstring>
	#include<vector>
	
	namespace acs
	{
		std::string runShell(std::vector<std::string> v, int flavor=1);
		std::string runShell(std::initializer_list< std::vector<std::string> > vlist={}, int flavor=1);
		std::string runShell(int flavor=1);
		
		std::string SuperInput(std::initializer_list< std::vector<std::string> > vlist={});
		std::string SuperInput(std::vector<std::string> v);
	}
	
#endif
