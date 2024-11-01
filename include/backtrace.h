#pragma once

#include <execinfo.h>
#include <iostream>
#include "namespace.h"

namespace TOPNSPC {

struct BackTrace {
	virtual ~BackTrace() {};
	virtual void print(std::ostream& out) const = 0;
};
inline std::ostream& operator<<(std::ostream& out, const BackTrace& bt) {
	bt.print(out);
	return out;
}

struct ClibBackTrace : public BackTrace {
	const static int max = 32;
	int skip;
	size_t size;
	void *array[max]{};
	char **strings;

	explicit ClibBackTrace(int s) {
		skip = s;
		size = backtrace(array, max);
		strings = backtrace_symbols(array, size);
	}
	~ClibBackTrace() {
		free(strings);
	}

	ClibBackTrace(const ClibBackTrace& other);
	const ClibBackTrace& operator=(const ClibBackTrace& other);
	
	void print(std::ostream& out) const override;
	static std::string demangle(const char* name);
};

}
