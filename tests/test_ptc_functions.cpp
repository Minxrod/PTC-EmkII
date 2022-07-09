#include "catch.hpp"

#include "Vars.hpp"
#include "BuiltinFuncs.hpp"

TEST_CASE("HEX$", "ptc_func"){
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
	
	//zero
	CHECK(hex1(0) == "0");
	CHECK(hex2(0,1) == "0");
	CHECK(hex2(0,2) == "00");
	CHECK(hex2(0,3) == "000");
	CHECK(hex2(0,4) == "0000");
	CHECK(hex2(0,5) == "00000");
}

TEST_CASE("RIGHT$", "ptc_func"){
	auto right = [](std::string s, double n) -> String {
		return std::get<String>(ptc::right({s,n}));
	};
	
	// regular
	CHECK(right("HELLO WORLD", 5) == "WORLD");
	
	// right none
	CHECK(right("HELLO WORLD", 0) == "");

	// past bound
	CHECK(right("HELLO WORLD", 15) == "HELLO WORLD");
}

TEST_CASE("LEFT$", "ptc_func"){
	auto left = [](std::string s, double n) -> String {
		return std::get<String>(ptc::left({s,n}));
	};
	
	// regular
	CHECK(left("HELLO WORLD", 5) == "HELLO");
	
	// left none
	CHECK(left("HELLO WORLD", 0) == "");

	// past bound
	CHECK(left("HELLO WORLD", 15) == "HELLO WORLD");
}

//technically an operator but whatever
TEST_CASE("NOT", "ptc_operator"){
	auto bnot = [](double n) -> Number {
		return std::get<Number>(ptc::bnot({n}));
	};
	
	//random
	CHECK(bnot(6172) == Approx(-6173));
	CHECK(bnot(4822) == Approx(-4823));
	CHECK(bnot(4519) == Approx(-4520));
	CHECK(bnot(7635) == Approx(-7636));
	CHECK(bnot(8169) == Approx(-8170));
	
	//random negative
	CHECK(bnot(-8084) == Approx(8083));
	CHECK(bnot(-5273) == Approx(5272));
	CHECK(bnot(-7744) == Approx(7743));
	CHECK(bnot(-1320) == Approx(1319));
	CHECK(bnot(-1784) == Approx(1783));
	
	//edges
	CHECK(bnot(0) == Approx(-1));
	CHECK(bnot(-1) == Approx(0));
	CHECK(bnot(524286) == Approx(-524287));
	CHECK(bnot(524287) == Approx(-0));
}

