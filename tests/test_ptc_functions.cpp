#include "catch.hpp"

#include "test_util.hpp"

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
	auto right = [](String s, double n) -> String {
		return std::get<String>(ptc::right({s,n}));
	};
	
	// regular
	CHECK(right(L"HELLO WORLD", 5) == L"WORLD");
	
	// right none
	CHECK(right(L"HELLO WORLD", 0) == L"");

	// past bound
	CHECK(right(L"HELLO WORLD", 15) == L"HELLO WORLD");
}

TEST_CASE("LEFT$", "ptc_func"){
	auto left = [](String s, double n) -> String {
		return std::get<String>(ptc::left({s,n}));
	};
	
	// regular
	CHECK(left(L"HELLO WORLD", 5) == L"HELLO");
	
	// left none
	CHECK(left(L"HELLO WORLD", 0) == L"");

	// past bound
	CHECK(left(L"HELLO WORLD", 15) == L"HELLO WORLD");
}

TEST_CASE("CHR$", "ptc_func"){
	auto chr = [](double n) -> String {
		return std::get<String>(ptc::chr({n}));
	};
	
	CHECK(chr(0) == String{L'\0'});
	CHECK(chr(65) == "A");
	CHECK(chr(97) == "a");
	CHECK(chr(128) == "\x80");
	CHECK(chr(255) == "\xff");
	
	CHECK_THROWS(chr(-1));
	CHECK_THROWS(chr(256));
	CHECK_THROWS(ptc::chr({}));
	CHECK_THROWS(ptc::chr({0.0, 0.0}));
}

TEST_CASE("ASC", "ptc_func"){
	auto asc = [](String s) -> Number {
		return std::get<Number>(ptc::asc({s}));
	};
	
	CHECK(asc(L"\0") == 0);
	CHECK(asc(L"\x10") == 0x10);
	CHECK(asc(L"\x27") == 0x27);
	CHECK(asc(L"\xdb") == 0xdb);
	CHECK(asc(L"\xe4") == 0xe4);
	CHECK(asc(L"\x89") == 0x89);
	CHECK(asc(L"\x17") == 0x17);
	CHECK(asc(L"\x53") == 0x53);
	CHECK(asc(L"\xff") == 0xff);
	// extended range check
	CHECK(asc(L"\x100") == 0x20);
	CHECK(asc(L"\x1df") == 0xff);
	CHECK(asc(L"\x1e0") == 0x00);
	CHECK(asc(L"\x1ff") == 0x1f);
	//TODO: 0x3000-0x30ff?
	
	CHECK(asc(L"ABC") == 'A');
	CHECK(asc(L";w5p gw324") == ';');
	CHECK(asc(L"\r3upf ") == '\r');
	CHECK(asc(L"vsaob") == 'v');
	CHECK(asc(L"\x95\x98\xff") == 0x95);
	CHECK_THROWS(asc(L""));
}

TEST_CASE("SQR", "ptc_func"){
	auto sqr = [](Number n) -> Number {
		return std::get<Number>(ptc::sqr({n}));
	};
	
	CHECK_THROWS(sqr(-1));
	CHECK(sqr(0) == Approx(0));
	CHECK(sqr(1) == Approx(1));
	CHECK(sqr(4) == Approx(2));
	CHECK(sqr(9) == Approx(3));
	
	CHECK_THROWS(ptc::sqr({0.0, 0.0}));
}

TEST_CASE("VAL", "ptc_func"){
	auto val = [](String s) -> Number {
		return std::get<Number>(ptc::val({s}));
	};
	
	CHECK(val(L"") == Approx(0));
	CHECK(val(L"0") == Approx(0));
	CHECK(val(L"1234") == Approx(1234));
	CHECK(val(L"56.7") == Approx(56.7));
	CHECK(val(L"-38") == Approx(-38));
	CHECK(val(L"-2059.54") == Approx(-2059.54));
	
	CHECK_THROWS(ptc::val({0.0}));
	CHECK_THROWS(ptc::val({L"0", L"0"}));
}

TEST_CASE("ABS", "ptc_func"){
	auto abs = [](double n) -> Number {
		return std::get<Number>(ptc::abs({n}));
	};
	
	CHECK(abs(0.0) == Approx(0.0));
	CHECK(abs(-524287.99) == Approx(524287.99));
	CHECK(abs(524287.99) == Approx(524287.99));
	CHECK(abs(1000.0) == Approx(1000.0));
	CHECK(abs(-1000.0) == Approx(1000.0));
	
	CHECK_THROWS(ptc::abs({}));
	CHECK_THROWS(ptc::abs({0.0, 0.0}));
}

TEST_CASE("ATAN", "ptc_func"){
	auto atan = wrap_ptc_func<Number>(ptc::atan);
	
	CHECK(atan({-25.1}) == Approx(-1.532).margin(1.0/4096));
	CHECK(atan({-0.3}) == Approx(-0.289).margin(1.0/4096));
	CHECK(atan({0.0}) == Approx(0).margin(1.0/4096));
	CHECK(atan({1.0}) == Approx(3217.0/4096).margin(1.0/4096));
	CHECK(atan({1.1}) == Approx(0.834).margin(1.0/4096));
	CHECK(atan({-7.4}) == Approx(-5893.0/4096).margin(1.0/4096));
	CHECK(atan({20.1}) == Approx(1.524).margin(1.0/4096));
	CHECK(atan({36.3}) == Approx(1.547).margin(1.0/4096));
	CHECK(atan({44.0}) == Approx(6370.0/4096).margin(1.0/4096));
	CHECK(atan({54.5}) == Approx(1.555).margin(1.0/4096));
	CHECK(atan({65.5}) == Approx(1.563).margin(1.0/4096));
	CHECK(atan({87.9}) == Approx(1.563).margin(1.0/4096));
	CHECK(atan({91.7}) == Approx(1.563).margin(1.0/4096));
	
	//2 arg version
	CHECK(atan({38.3, -36.9}) == Approx(9569.0/4096).margin(1.0/4096));
	CHECK(atan({4.6, -38.8}) == Approx(12390.0/4096).margin(1.0/4096));
	CHECK(atan({43.6, 10.0}) == Approx(5521.0/4096).margin(1.0/4096));
	CHECK(atan({0.0, 0.0}) == Approx(0.0).margin(1.0/4096));
	
	CHECK_THROWS(atan({}));
	CHECK_THROWS(atan({0.0, 0.0, 0.0}));
}

TEST_CASE("COS", "ptc_func"){
	auto cos = wrap_ptc_func<Number>(ptc::cos);
	
	CHECK(cos({2.19}) == Approx(-2378.0/4096).margin(1.0/4096));
	CHECK(cos({4.16}) == Approx(-2150.0/4096).margin(1.0/4096));
	CHECK(cos({-3.3}) == Approx(-4045.0/4096).margin(1.0/4096));
	CHECK(cos({-4.85}) == Approx(562.0/4096).margin(1.0/4096));
	CHECK(cos({1.47}) == Approx(412.0/4096).margin(1.0/4096));
	CHECK(cos({0.0}) == Approx(1.0).margin(1.0/4096));
	
	CHECK_THROWS(cos({}));
	CHECK_THROWS(cos({0.0, 0.0}));
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

TEST_CASE("AND", "ptc_operator"){
	auto band = [](double a, double b) -> Number {
		return std::get<Number>(ptc::band({a, b}));
	};
	
	CHECK(band(12345.0, 56789.0) == Approx(4113));
	CHECK(band(12345.0, -56789.0) == Approx(8233));
	CHECK(band(-12345.0, 56789.0) == Approx(52677));
	CHECK(band(-12345.0, -56789.0) == Approx(-65021));

	CHECK(band(99999.0, 87654.0) == Approx(67078));
	CHECK(band(-99999.0, -87654.0) == Approx(-120576));
	CHECK(band(-99999.0, 87654.0) == Approx(20576));
	CHECK(band(99999.0, -87654.0) == Approx(32922));
}
