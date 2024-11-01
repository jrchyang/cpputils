#pragma once

#include <time.h>
#include "namespace.h"
#include "utime.h"

namespace TOPNSPC {

static inline utime_t cpputils_clock_now() {
	struct timespec tp;

	clock_gettime(CLOCK_REALTIME, &tp);
	utime_t n(tp);
	return n;
}

}
