#include <iostream>
#include <cassert>
#include <stdarg.h>
#include "clock.h"
#include "dout.h"
#include "backtrace.h"
#include "assert.h"

namespace TOPNSPC {

[[gnu::cold]] void
__cpputils_assert_fail(const char *assertion, const char *file, int line, const char *func)
{
	char assert_msg[8096] = { 0 };
	std::ostringstream tss;
	tss << cpputils_clock_now();

	snprintf(assert_msg, sizeof(assert_msg),
		 "%s: In function '%s' thread %llx time %s\n"
		 "%s: %d: FAILED cpputils_assert(%s)\n",
		 file, func, (unsigned long long)pthread_self(),
		 tss.str().c_str(), file, line, assertion);
	dout_emergency(assert_msg);

	std::ostringstream oss;
	oss << ClibBackTrace(1);
	dout_emergency(oss.str());

	abort();
}

[[gnu::cold]] void __cpputils_assert_fail(const assert_data &ctx)
{
	__cpputils_assert_fail(ctx.assertion, ctx.file, ctx.line, ctx.function);
}

class BufAppender {
public:
	BufAppender(char* buf, int size) : bufptr(buf), remaining(size) {}

	void printf(const char * format, ...) {
		va_list args;
		va_start(args, format);
		this->vprintf(format, args);
		va_end(args);
	}

	void vprintf(const char * format, va_list args) {
		int n = vsnprintf(bufptr, remaining, format, args);
		if (n >= 0) {
			if (n < remaining) {
				remaining -= n;
				bufptr += n;
			} else {
				remaining = 0;
			}
		}
	}

private:
	char* bufptr;
	int remaining;
};

[[gnu::cold]] void
__cpputils_assertf_fail(const char *assertion, const char *file, int line,
			const char *func, const char* msg, ...)
{
	char assert_msg[8096] = { 0 };
	std::ostringstream tss;
	tss << cpputils_clock_now();

	BufAppender ba(assert_msg, sizeof(assert_msg));
	BackTrace *bt = new ClibBackTrace(1);
	ba.printf("%s: In function '%s' thread %llx time %s\n"
		  "%s: %d: FAILED cpputilities_assert(%s)\n",
		  file, func, (unsigned long long)pthread_self(),
		  tss.str().c_str(), file, line, assertion);
	ba.printf("Assertion details: ");
	va_list args;
	va_start(args, msg);
	ba.vprintf(msg, args);
	va_end(args);
	ba.printf("\n");
	dout_emergency(assert_msg);

	std::ostringstream oss;
	oss << *bt;
	dout_emergency(oss.str());

	abort();
}

[[gnu::cold]] void
__cpputils_abort(const char *file, int line, const char *func, const std::string& msg)
{
	char assert_msg[8096] = { 0 };
	std::ostringstream tss;
	tss << cpputils_clock_now();

	BackTrace *bt = new ClibBackTrace(1);
	snprintf(assert_msg, sizeof(assert_msg),
		 "%s: In function '%s' thread %llx time %s\n"
		 "%s: %d: cpputilities_abort_msg(\"%s\")\n",
		 file, func, (unsigned long long)pthread_self(),
		 tss.str().c_str(), file, line, msg.c_str());
	dout_emergency(assert_msg);

	std::ostringstream oss;
	oss << *bt;
	dout_emergency(oss.str());

	abort();
}

[[gnu::cold]] void
__cpputils_abortf(const char *file, int line, const char *func, const char* msg, ...)
{
	char assert_msg[8096] = { 0 };
	std::ostringstream tss;
	tss << cpputils_clock_now();

	BufAppender ba(assert_msg, sizeof(assert_msg));
	BackTrace *bt = new ClibBackTrace(1);
	ba.printf("%s: In function '%s' thread %llx time %s\n"
		  "%s: %d: abort()\n",
		  file, func, (unsigned long long)pthread_self(),
		  tss.str().c_str(), file, line);
	ba.printf("Abort details: ");
	va_list args;
	va_start(args, msg);
	ba.vprintf(msg, args);
	va_end(args);
	ba.printf("\n");
	dout_emergency(assert_msg);

	std::ostringstream oss;
	oss << *bt;
	dout_emergency(oss.str());

	abort();
}

}
