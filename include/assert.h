#pragma once

#include <cstdlib>
#include <string>
#include <features.h>
#include "namespace.h"

/*
 * Select a function-name variable based on compiler tests, and any compiler
 * specific overrides.
 */
#if defined(HAVE_PRETTY_FUNC)
# define __CPPUTILS_ASSERT_FUNCTION __PRETTY_FUNCTION__
#elif defined(HAVE_FUNC)
# define __CPPUTILS_ASSERT_FUNCTION __func__
#else
# define __CPPUTILS_ASSERT_FUNCTION ((__const char *) 0)
#endif

namespace TOPNSPC {

struct assert_data {
	const char *assertion;
	const char *file;
	const int   line;
	const char *function;
};

extern void
__cpputils_assert_fail(const char *assertion, const char *file, int line,
		       const char *function) __attribute__ ((__noreturn__));
extern void
__cpputils_assert_fail(const assert_data &ctx) __attribute__ ((__noreturn__));
extern void
__cpputils_assertf_fail(const char *assertion, const char *file, int line,
			const char *function, const char* msg,
			...) __attribute__ ((__noreturn__));

[[noreturn]] void __cpputils_abort(const char *file, int line, const char *func, const std::string& msg);
[[noreturn]] void __cpputils_abortf(const char *file, int line, const char *func, const char* msg, ...);

}

#define cpputils_abort(msg, ...)						\
  ::TOPNSPC::__cpputils_abort(__FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION, "abort() called")
#define cpputils_abort_msg(msg)							\
  ::TOPNSPC::__cpputils_abort(__FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION, msg)
#define cpputils_abort_msgf(...)						\
  ::TOPNSPC::__cpputils_abortf(__FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION, __VA_ARGS__)

#define cpputils_assert(expr)							\
  do {										\
    static const ::TOPNSPC::assert_data assert_data_ctx =			\
      {__STRING(expr), __FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION};		\
    ((expr)									\
      ? static_cast<void> (0)							\
      : ::TOPNSPC::__cpputils_assert_fail(assert_data_ctx));			\
  } while(false)

#define cpputils_assert_always(expr)						\
  do {										\
    static const ::TOPNSPC::assert_data assert_data_ctx =			\
      {__STRING(expr), __FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION};		\
    ((expr)									\
      ? static_cast<void> (0)							\
      : ::TOPNSPC::__cpputils_assertf_fail(assert_data_ctx));			\
  } while(false)

#define cpputils_assertf(expr, ...)						\
  ((expr)									\
    ? static_cast<void> (0)							\
    : ::TOPNSPC::__cpputils_assertf_fail(__STRING(expr), __FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION, __VA_ARGS__))

#define cpputils_assertf_always(expr, ...)					\
  ((expr)									\
    ? static_cast<void> (0)							\
    : ::TOPNSPC::__cpputils_assertf_fail(__STRING(expr), __FILE__, __LINE__, __CPPUTILS_ASSERT_FUNCTION, __VA_ARGS__))


