#include<iostream>
#include<limits>

void raise(std::string errMsg, int exitCode)
{
	std::cerr<<errMsg; exit(exitCode);
}

template<typename T>
struct MaxBit
{
	static T value()
	{
		T x = std::numeric_limits<T>::max();
		return x ^ (x>>1);
	}
};
