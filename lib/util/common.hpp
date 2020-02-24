#ifndef ACShell_UTIL_COMMON_HPP
#define ACShell_UTIL_COMMON_HPP

void raise(std::string errMsg, int exitCode);

template<typename T>
struct MaxBit
{
	static T value();
};

#endif
