#include <ios>
#include <string>
#include <sstream>
#include <regex>

#include "gtest/gtest.h"
#include "cpputils.h"

using namespace cpputils;

TEST(Miscs, Version) {
	std::string s = pretty_version_to_str();
	std::regex p(R"(cpputils version)");
	EXPECT_TRUE(std::regex_search(s, p));
}
