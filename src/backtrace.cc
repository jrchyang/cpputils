#include <cxxabi.h>
#include "version.h"
#include "backtrace.h"

namespace TOPNSPC {

std::string ClibBackTrace::demangle(const char *name)
{
	// find the parentheses and address offset surrounding the mangled name
	static constexpr char OPEN = '(';
	const char *begin = nullptr;
	const char *end = nullptr;

	for (const char *j = name; *j; ++j) {
		if (*j == OPEN) {
			begin = j + 1;
		} else if (*j == '+') {
			end = j;
		}
	}

	if (begin && end && begin < end) {
		int status;
		std::string mangled(begin, end);

		// only demangle a C++ mangled name
		if (mangled.compare(0, 2, "_Z") == 0) {
			// let __cxa_demangle do the malloc
			char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
			if (!status) {
				std::string full_name{OPEN};
				full_name += demangled;
				full_name += end;
				// buf could be reallocated, so free(demangled) instead
				free(demangled);
				return full_name;
			}
			// demangle failed, just pretend it's a C function with no args
		}
		// C function
		return mangled + "()";
	} else {
		// didn't find the mangled name, just print the whole line
		return name;
	}
}

void ClibBackTrace::print(std::ostream &out) const
{
	out << " " << pretty_version_to_str() << std::endl;
	for (size_t i = skip; i < size; ++i) {
		out << " " << (i-skip+1) << ": "
		    << demangle(strings[i]) << std::endl;
	}
}

}
