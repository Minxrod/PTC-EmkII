#include "catch.hpp"

#include "test_util.hpp"

#include "Tokens.hpp"
#include "Evaluator.hpp"


TEST_CASE("Evaluate MM2PTC:2711", "evaluate"){
	//":DCMPS=0OR(DCMPSIZE/256)+!!(DCMPSIZE%256)");
	
	Evaluator e;
	e.evaluate(tokenize_str("DCMPSIZE=3000"));
	
	REQUIRE(std::get<Number>(e.evaluate(tokenize_str("DCMPSIZE"))) == 3000);
	
	e.evaluate(tokenize_str("DCMPS=0OR(DCMPSIZE/256)+!!(DCMPSIZE%256)"));
	
	REQUIRE(std::get<Number>(e.evaluate(tokenize_str("DCMPSIZE/256"))) == Approx(11.719).epsilon(1/4096.0));
	REQUIRE(std::get<Number>(e.evaluate(tokenize_str("DCMPSIZE%256"))) == 184);
	REQUIRE(std::get<Number>(e.evaluate(tokenize_str("!!(DCMPSIZE%256)"))) == 1);
	REQUIRE(std::get<Number>(e.evaluate(tokenize_str("0OR(11.719)"))) == 11);
	
	REQUIRE(std::get<Number>(e.evaluate(tokenize_str("DCMPS"))) == 12);
	
	
}

TEST_CASE("Randomized", "evaluate"){
	Evaluator e;
	
	auto eval = [&e](std::string expr){
		return std::get<Number>(e.evaluate(tokenize_str(expr)));
	};
	
	CHECK(eval("SQR(1478 % 1579)") == Approx(38.445));
}

TEST_CASE("SAMPLE5 (207)", "evaluate"){
	Evaluator e;
	
	auto eval = [&e](std::string expr){
		return std::get<Number>(e.evaluate(tokenize_str(expr)));
	};
	
	CHECK(eval("64 <= 72 AND 72 < 48") == Approx(0.0));
	CHECK(eval("64 >= 72 AND 72 > 48") == Approx(0.0));
	CHECK(eval("48 <= 72 AND 72 < 64") == Approx(0.0));
	CHECK(eval("48 >= 72 AND 72 > 64") == Approx(0.0));
}
