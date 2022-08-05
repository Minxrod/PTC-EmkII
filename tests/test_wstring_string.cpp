#include "catch.hpp"

#include "Vars.hpp"

TEST_CASE("String comparisons", "string"){
	// Check that there aren't any more missing conditions in the string checks
	REQUIRE(String(L"12345") == "12345");
	REQUIRE(String(L"123") != "12345");
	REQUIRE(String(L"1234567") != "12345");
	REQUIRE(String(L"\0") == "\0");
	
	std::string str = "12345";
	std::string null = "\0";
	REQUIRE(String(L"12345") == const_cast<std::string&>(str));
	REQUIRE_FALSE(String(L"123") == str);
	REQUIRE_FALSE(String(L"1234567") == str);
	REQUIRE(String(L"\0") == null);
}
