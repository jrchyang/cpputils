#include <stdlib.h>
#include <sstream>
#include "version.h"

#define _CPPUTILS_STR(x) #x
#define CPPUTILS_STRINGIFY(x) _CPPUTILS_STR(x)

namespace TOPNSPC {

std::string const pretty_version_to_str(void)
{
	std::ostringstream oss;
	oss << "cpputils version " << CPPUTILS_GIT_NICE_VER
	    << " (" << CPPUTILS_STRINGIFY(CPPUTILS_GIT_VER) << ") ";
	return oss.str();
}

}
