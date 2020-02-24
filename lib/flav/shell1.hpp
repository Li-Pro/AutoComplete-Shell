#ifndef ACShell_FLAV_SHELL1_HPP
#define ACShell_FLAV_SHELL1_HPP

#include "shell_base.hpp"

struct ShF1: ShellBase
{
	std::string shell(std::vector<std::string> suggestion={}) override;
};

#endif
