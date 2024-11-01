#include <iostream>
#include "dout.h"

namespace TOPNSPC {

void dout_emergency(const char * const str)
{
	std::cerr << str;
	std::cerr.flush();
}

void dout_emergency(const std::string &str)
{
	std::cerr << str;
	std::cerr.flush();
}

}
