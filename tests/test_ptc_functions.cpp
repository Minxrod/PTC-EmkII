#include "catch.hpp"

#include "Vars.hpp"
#include "BuiltinFuncs.hpp"

TEST_CASE("hex", "ptc_func"){
	auto hex2 = [](double n, double d) -> String {
		return std::get<String>(ptc::hex({n,d}));
	};
	
	auto hex1 = [](double n) -> String {
		return std::get<String>(ptc::hex({n}));
	};
	
	// misc positive, negative one-argument
	CHECK(hex1(9) == "9");
	CHECK(hex1(29.7) == "1D");
	CHECK(hex1(89.6) == "59");
	CHECK(hex1(-5) == "FFFFB");
	CHECK(hex1(-967) == "FFC39");
	CHECK(hex1(-2585.6) == "FF5E6");
	
	// positive with digits
	CHECK(hex2(9,1) == "9");
	CHECK(hex2(9,2) == "09");
	CHECK(hex2(9,3) == "009");
	CHECK(hex2(9,4) == "0009");
	CHECK(hex2(9,5) == "00009");
	
	// negatives with digits
	CHECK_THROWS(hex2(-9,1));
	CHECK(hex2(-9,2) == "F7");
	CHECK(hex2(-9,3) == "FF7");
	CHECK(hex2(-9,4) == "FFF7");
	CHECK(hex2(-9,5) == "FFFF7");
	
	// edge cases 1-argument
	CHECK(hex1(-524287) == "80001");
	CHECK(hex1(-524287.8) == "80000");
	CHECK(hex1(524287) == "7FFFF");
	CHECK(hex1(524287.8) == "7FFFF");
	
	// edge cases 2-argument
	CHECK(hex2(-8,1) == "8");
	CHECK(hex2(15,1) == "F");
	CHECK_THROWS(hex2(16,1) == "8");
}
