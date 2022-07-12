#include "catch.hpp"

#include "test_util.hpp"

#include "Tokens.hpp"

TEST_CASE("Tokenize (GRPED:127)", "tokenize"){
	auto tokens = tokenize_str("K=B AND (NOT O)\r");
	
	REQUIRE(tokens.size() == 9);
	
	REQUIRE(tokens[0] == Token{"K", Type::Var});
	REQUIRE(tokens[1] == Token{"=", Type::Op});
	REQUIRE(tokens[2] == Token{"B", Type::Var});
	REQUIRE(tokens[3] == Token{"AND", Type::Op});
	REQUIRE(tokens[4] == Token{"(", Type::Op});
	REQUIRE(tokens[5] == Token{"NOT", Type::Op});
	REQUIRE(tokens[6] == Token{"O", Type::Var});
	REQUIRE(tokens[7] == Token{")", Type::Op});
	REQUIRE(tokens[8] == Token{"\r", Type::Newl});
}

TEST_CASE("Tokenize (NOT O)", "tokenize"){
	auto tokens = tokenize_str("NOT O");
	
	REQUIRE(tokens.size() == 2);
	
	REQUIRE(tokens[0] == Token{"NOT", Type::Op});
	REQUIRE(tokens[1] == Token{"O", Type::Var});
}

TEST_CASE("Tokenize (MM2PTC:2711)", "tokenize"){
	auto tokens = tokenize_str("DCMPSIZE=3000:DCMPS=0OR(DCMPSIZE/256)+!!(DCMPSIZE%256)");
	
	REQUIRE(tokens.size() == 21);
	
	REQUIRE(tokens[0] == Token{"DCMPSIZE", Type::Var});
	REQUIRE(tokens[1] == Token{"=", Type::Op});
	REQUIRE(tokens[2] == Token{"3000", Type::Num});
	REQUIRE(tokens[3] == Token{":", Type::Newl});
	REQUIRE(tokens[4] == Token{"DCMPS", Type::Var});
	REQUIRE(tokens[5] == Token{"=", Type::Op});
	REQUIRE(tokens[6] == Token{"0", Type::Num});
	REQUIRE(tokens[7] == Token{"OR", Type::Op});
	REQUIRE(tokens[8] == Token{"(", Type::Op});
	REQUIRE(tokens[9] == Token{"DCMPSIZE", Type::Var});
	REQUIRE(tokens[10] == Token{"/", Type::Op});
	REQUIRE(tokens[11] == Token{"256", Type::Num});
	REQUIRE(tokens[12] == Token{")", Type::Op});
	REQUIRE(tokens[13] == Token{"+", Type::Op});
	REQUIRE(tokens[14] == Token{"!", Type::Op});
	REQUIRE(tokens[15] == Token{"!", Type::Op});
	REQUIRE(tokens[16] == Token{"(", Type::Op});
	REQUIRE(tokens[17] == Token{"DCMPSIZE", Type::Var});
	REQUIRE(tokens[18] == Token{"%", Type::Op});
	REQUIRE(tokens[19] == Token{"256", Type::Num});
	REQUIRE(tokens[20] == Token{")", Type::Op});
}
