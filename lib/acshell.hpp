#ifndef ACShell_HPP
#define ACShell_HPP
	
	#include<cstring>
	#include<vector>
//	std::string runShell(std::vector<std::string> pool);
//	template<typename... T>
//	std::string runShell(std::vector<std::string> v1, std::vector<std::string> v2, T... velse);
//	std::string runShell(std::vector<std::string> v1, std::vector<std::string> v2);
//	std::string runShell(std::vector<std::string> v1);
//	std::string runShell();
	template
	<typename... T/*,
	 typename = typename std::enable_if< std::is_same<T, std::vector<std::string>>::value >*/>
	std::string runShell(T... v);
	
	
//	std::string SuperInput(std::vector<std::string> pool={});
	template<typename... T2>
	std::string SuperInput(T2... pool);
	
#endif
