#include <string>
#include <sstream>
#include <regex>
#include <cassert>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "gtest/gtest.h"
#include "cpputils.h"

using namespace cpputils;

TEST(Miscs, Version) {
	std::string s = pretty_version_to_str();
	std::regex p(R"(cpputils version)");
	EXPECT_TRUE(std::regex_search(s, p));
}

// a dummy function, so we can check "foo" in the backtrace.
// do not mark this function as static or put it into an anonymous namespace,
// otherwise it's function name will be removed in the backtrace.
static std::string foo()
{
  std::ostringstream oss;
  oss << ClibBackTrace(0);
  return oss.str();
}

TEST(Miscs, BackTrace) {
	std::string bt = foo();
	std::vector<std::string> lines;
	boost::split(lines, bt, boost::is_any_of("\n"));
	const unsigned lineno = 1;
	std::regex pattern(R"(^ 1: .*\[0x[0-9a-fA-F]+\])");

	ASSERT_GT(lines.size(), lineno);
	ASSERT_EQ(lines[0].find(pretty_version_to_str()), 1U);
	EXPECT_TRUE(std::regex_match(lines[lineno], pattern));
}

TEST(Miscs, Clock) {
	std::ostringstream tss;
	tss << cpputils_clock_now();
	std::string time = tss.str();
	std::regex pattern(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01]).*)");
	EXPECT_TRUE(std::regex_match(time, pattern));
}

TEST(Miscs, Assert) {
	EXPECT_DEATH({ cpputils_abort("common test"); }, ".*");
	EXPECT_DEATH({ cpputils_abort_msg("common test"); }, ".*");
	cpputils_assert(1);
}

TEST(Miscs, Formatter) {
	struct stats_t {
		ssize_t items = 10;
		ssize_t bytes = 1240;

		void dump(Formatter *f) const {
			f->open_object_section("MiscsFormatter");
			f->dump_int("items", items);
			f->dump_int("bytes", bytes);
			f->close_section();
		}
	} s;
	auto f = Formatter::create("json-pretty");
	s.dump(f);
	std::stringstream oss;
	f->flush(oss);
	delete f;
}
